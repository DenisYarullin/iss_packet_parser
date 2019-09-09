#include "receiver.h"
#include "utility.h"

PacketReceiver::PacketReceiver(std::shared_ptr<ICallback> callback) : callback_(callback)
{
	t_ = std::thread([this]() { WorkWithData(); });
}


PacketReceiver::~PacketReceiver()
{
	state_.cancel_ = true;
	cv_.notify_one();
	t_.join();
}


void PacketReceiver::Receive(const char* data, size_t size)
{
	{
		std::lock_guard<std::mutex> lk(state_.mu_);
		char* block = new char[size];
		memcpy(&block[0], data, size);
		state_.blockBuffer_.emplace(block, size);
	}
	cv_.notify_one();
}


void PacketReceiver::WorkWithData() noexcept
{
	while (true)
	{
		std::unique_lock<std::mutex> lk(state_.mu_);
		cv_.wait(lk, [this]() { return !state_.blockBuffer_.empty() || state_.cancel_; });
		if (state_.cancel_ && state_.blockBuffer_.empty())
			return;
		Block dataBlock = std::move(state_.blockBuffer_.front());
		state_.blockBuffer_.pop();
		lk.unlock();

		ParseAndSendPacket(dataBlock);
	}
}


void PacketReceiver::ParseAndSendPacket(const Block& block) const noexcept
{
	std::vector<char>& blockData = tempPacket.data_;

	auto& [data, size] = block;
	auto blockSize = blockData.size() + size;
	blockData.resize(blockSize);
	std::copy(data, data + size, std::end(blockData) - size);

	while (blockSize > 0)
	{
		bool isCompletePacket = false;
		size_t textPacketTailIndex = 0;
		if (tempPacket.type_ == PacketType::None)
		{
			const char* binaryPacketHeaderAddress = static_cast<const char*>(memchr(blockData.data(), tempPacket.binaryPacketHeaderByte_, blockSize));
			const char* textPacketTailAddress = memstr(blockData.data(), tempPacket.textPacketTailString_, blockSize);

			if (binaryPacketHeaderAddress != nullptr && textPacketTailAddress != nullptr)	// в блоке есть и бинарный (или часть), и текстовый пакет
			{
				const size_t binaryPacketHeaderIndex = binaryPacketHeaderAddress - blockData.data();
				textPacketTailIndex = textPacketTailAddress - blockData.data();
				tempPacket.type_ = (binaryPacketHeaderIndex == 0) ? PacketType::Binary : PacketType::Text; // бинарный пакет начинается с нулевого индекса			
			}
			else if (binaryPacketHeaderAddress != nullptr)
				tempPacket.type_ = PacketType::Binary;
			else if (textPacketTailAddress != nullptr)
			{
				tempPacket.type_ = PacketType::Text;
				textPacketTailIndex = textPacketTailAddress - blockData.data();
			}
			else // блок текстовый, но нет признака конца
				break;
		}

		if (tempPacket.type_ == PacketType::Binary)
		{
			if (tempPacket.binaryPacketOffset_ <= blockSize)	// в блок влезло хотя бы значение размера пакета
			{
				const auto binaryPacketSize = convertBytesToUintLittleEndian(&blockData[1]);
				if (tempPacket.binaryPacketOffset_ + binaryPacketSize <= blockSize) // в блок влез весь пакет
				{
					if (binaryPacketSize == 0)
						callback_->BinaryPacket("", binaryPacketSize);
					else
						callback_->BinaryPacket(&blockData[tempPacket.binaryPacketOffset_], binaryPacketSize);
					const size_t nextPacketHeadIndex = tempPacket.binaryPacketOffset_ + binaryPacketSize;
					blockSize -= nextPacketHeadIndex;
					std::move(std::begin(blockData) + nextPacketHeadIndex, std::begin(blockData) + nextPacketHeadIndex + blockSize, std::begin(blockData));
					blockData.resize(blockSize);
					isCompletePacket = true;
					tempPacket.type_ = PacketType::None;
				}
			}
		}
		else if (tempPacket.type_ == PacketType::Text)
		{
			const auto textPacketSize = textPacketTailIndex;
			callback_->TextPacket(blockData.data(), textPacketSize);
			const size_t nextPacketHeadIndex = tempPacket.textPacketOffset_ + textPacketSize;
			blockSize -= nextPacketHeadIndex;
			std::move(std::begin(blockData) + nextPacketHeadIndex, std::begin(blockData) + nextPacketHeadIndex + blockSize, std::begin(blockData));
			blockData.resize(blockSize);
			isCompletePacket = true;
			tempPacket.type_ = PacketType::None;
		}

		if (!isCompletePacket)
			break;
	}
}

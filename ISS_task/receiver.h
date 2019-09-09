#pragma once

#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include "callback.h"

struct IReceiver
{
	virtual ~IReceiver() = default;
	virtual void Receive(const char* data, size_t size) = 0;
};

enum class PacketType { None, Binary, Text };

using Block = std::pair<const char*, size_t>;

struct PacketData
{
	std::vector<char> data_;
	size_t size_{};
	const size_t uintSize_ = 4;
	const size_t headerSize_ = 1;
	const size_t binaryPacketOffset_ = uintSize_ + headerSize_;
	const size_t textPacketOffset_ = 4;
	const char binaryPacketHeaderByte_ = 0x24;
	const char* textPacketTailString_ = "\r\n\r\n";
};

class PacketReceiver : public IReceiver
{
	void WorkWithData() noexcept;
	void ParseAndSendPacket(const Block& block) const noexcept;
public:
	PacketReceiver(std::shared_ptr<ICallback> callback);
	PacketReceiver() = default;
	PacketReceiver(const PacketReceiver&&) = delete;
	PacketReceiver(PacketReceiver&&) = delete;
	PacketReceiver& operator=(const PacketReceiver&) = delete;
	PacketReceiver& operator=(PacketReceiver&&) = delete;
	~PacketReceiver();
	void Receive(const char* data, size_t size) override;

private:
	struct
	{
		std::mutex mu_;
		std::queue<Block> blockBuffer_;
		std::atomic<bool> cancel_ = false;
	} state_;

	std::thread t_;
	std::condition_variable cv_;
	std::shared_ptr<ICallback> callback_;
	mutable PacketData tempPacket{};
};

#endif
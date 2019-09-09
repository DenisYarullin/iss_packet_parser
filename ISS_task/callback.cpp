#include "callback.h"
#include <iostream>

void PacketCallback::BinaryPacket(const char* data, size_t size)
{
	//std::cout << "Binary data!" << std::endl;
	//std::cout << "Size: " << size << std::endl;
	//std::cout << "Values: ";
	//for (size_t i = 0; i < size; i++)
	//	std::cout << static_cast<int>(data[i]) << " ";
	//std::cout << std::endl;
	binaryPackets_.emplace_back(data, size);
}


void PacketCallback::TextPacket(const char* data, size_t size)
{
	//std::cout << "Text data!" << std::endl;
	//std::cout << "Size: " << size << std::endl;
	//std::cout << "Values: ";
	//for (size_t i = 0; i < size; i++)
	//	std::cout << data[i] << " ";
	//std::cout << std::endl;
	textPackets_.emplace_back(data, size);
}


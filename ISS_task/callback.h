#pragma once

#ifndef CALLBACK_H_
#define CALLBACK_H_

#include <string>
#include <vector>

struct ICallback
{
	virtual ~ICallback() = default;
	virtual void BinaryPacket(const char* data, size_t size) = 0;
	virtual void TextPacket(const char* data, size_t size) = 0;
};

struct PacketCallback : public ICallback
{
	std::vector<std::string> binaryPackets_;
	std::vector<std::string> textPackets_;

	void BinaryPacket(const char* data, size_t size) override;
	void TextPacket(const char* data, size_t size) override;
};

#endif

#include "gtest/gtest.h"
#include "../ISS_task/callback.h"
#include "../ISS_task/receiver.h"

// пустой блок
TEST(ISS_TASK, NoPacketsEmptyBlock)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("", 0);
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.empty());
}

// блок, в котором только двоичный пакет
TEST(ISS_TASK, BinaryPacket_1Block)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("\x24\x0A\x00\x00\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A", 15);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\x1\x2\x3\x4\x5\x6\a\b\t\n"));
	EXPECT_TRUE(callback->textPackets_.empty());
}

// блок, в котором только текстовый пакет
TEST(ISS_TASK, TextPacket_1Block)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("abcdefghijklmn!\r\n\r\n", 19);
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("abcdefghijklmn!"));
}

// блок: бинарный-бинарный
TEST(ISS_TASK, TwoBinaryPackets_1Block)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("\x24\x0F\x00\x00\x00\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x24\x0A\x00\x00\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A", 35);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 2);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"));
	EXPECT_TRUE(callback->binaryPackets_[1] == std::string("\x1\x2\x3\x4\x5\x6\a\b\t\n"));
	EXPECT_TRUE(callback->textPackets_.empty());
}

// блок: текстовый-текстовый
TEST(ISS_TASK, TwoTextPackets_1Block)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("abcdefghijklmn!\r\n\r\nopqrstuvw!\r\n\r\n", 33);
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.size() == 2);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("abcdefghijklmn!"));
	EXPECT_TRUE(callback->textPackets_[1] == std::string("opqrstuvw!"));
}

// блок: текстовый-бинарный
TEST(ISS_TASK, TextBinaryPacket_1Block)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("abcdefghijklmn!\r\n\r\n$\n\0\0\0\x1\x2\x3\x4\x5\x6\a\b\t\n", 34);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\x1\x2\x3\x4\x5\x6\a\b\t\n"));
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("abcdefghijklmn!"));
}

//блок: бинарный-текстовый
TEST(ISS_TASK,BinaryTextPacket_1Block)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("$\n\0\0\0\x1\x2\x3\x4\x5\x6\a\b\t\nabcdefghijklmn!\r\n\r\n", 34);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\x1\x2\x3\x4\x5\x6\a\b\t\n"));
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("abcdefghijklmn!"));
}

// блок: текстовый-бинарный-текстовый
TEST(ISS_TASK, TextBinaryText_1Block)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("opqrstuvw!\r\n\r\n""$\xf\0\0\0\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19""abcdefghijklmn!\r\n\r\n", 53);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"));
	EXPECT_TRUE(callback->textPackets_.size() == 2);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("opqrstuvw!"));
	EXPECT_TRUE(callback->textPackets_[1] == std::string("abcdefghijklmn!"));
}

// блок: бинарный-текстовый-бинарный
TEST(ISS_TASK, BinaryTextBinary_1Block)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("$\xf\0\0\0\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19""opqrstuvw!\r\n\r\n""$\n\0\0\0\x1\x2\x3\x4\x5\x6\a\b\t\n", 49);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 2);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"));
	EXPECT_TRUE(callback->binaryPackets_[1] == std::string("\x1\x2\x3\x4\x5\x6\a\b\t\n"));
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("opqrstuvw!"));
}

// блок 1: часть бинарного, только часть данных размера; блок 2: все остальное
TEST(ISS_TASK, Binary_2Blocks)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("$\xf\0", 3);
		receiver.Receive("\0\0\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19", 17);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"));
	EXPECT_TRUE(callback->textPackets_.empty());
}

// бинарный пакет в двух блоках, блок 1: часть бинарного, только размер и немного данных; блок 2: все остальное
TEST(ISS_TASK, Binary_2Blocks2)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("$\xf\0\0\0\v", 6);
		receiver.Receive("\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19", 14);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"));
	EXPECT_TRUE(callback->textPackets_.empty());
}

// бинарный пакет в двух блоках, блок 1: только заголовок, блок 2: все остальное
TEST(ISS_TASK, Binary_2Blocks3)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("$", 1);
		receiver.Receive("\xf\0\0\0\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19", 19);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"));
	EXPECT_TRUE(callback->textPackets_.empty());
}

// бинарный пакет в трех блоках
TEST(ISS_TASK, Binary_3Blocks)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("$\xf\0", 3);
		receiver.Receive("\0\0\v\f\r\xe\xf\x10\x11\x12\x13", 11);
		receiver.Receive("\x14\x15\x16\x17\x18\x19", 6);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"));
	EXPECT_TRUE(callback->textPackets_.empty());
}

// блок 1: часть текстового; блок 2: все остальное
TEST(ISS_TASK, Text_2Blocks)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("abcdefghi", 9);
		receiver.Receive("jklmn!\r\n\r\n", 10);
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("abcdefghijklmn!"));
}

// текстовый пакет в двух блоках, блок 1: данные; блок 2: "\r\n\r\n"
TEST(ISS_TASK, Text_2Blocks2)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("abcdefghijklmn!", 15);
		receiver.Receive("\r\n\r\n", 4);
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("abcdefghijklmn!"));
}

// текстовый пакет в двух блоках, блок 1: данные + "\r\n\; блок 2: "\r\n\"
TEST(ISS_TASK, Text_2Blocks3)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("abcdefghijklmn!\r\n", 17);
		receiver.Receive("\r\n", 2);
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("abcdefghijklmn!"));
}

// текстовый пакет в трех блоках
TEST(ISS_TASK, Text_3Blocks)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("abcdef", 6);
		receiver.Receive("ghijk", 5);
		receiver.Receive("lmn!\r\n\r\n", 8);
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("abcdefghijklmn!"));
}

// различный
TEST(ISS_TASK, Mixed)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("$\xf\0", 3);
		receiver.Receive("\0\0\v\f\r\xe\xf\x10\x11\x12\x13", 11);
		receiver.Receive("\x14\x15\x16\x17\x18\x19""abcdefghijklmn!\r\n\r\n""opqrst", 31);
		receiver.Receive("uvw!\r\n\r\n""\x24\x0A\x00\x00\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A""\x24\x08\x00\x00", 27);
		receiver.Receive("\x00\x0F\x0A\x0B\x0C\x0D\x0E\x01\x0F", 9);
		receiver.Receive("Hello World!!\r\n\r\n", 17);
		receiver.Receive("\x24\x03\x00\x00\x00""!!!!!!!!!!\r\n\r\n", 19);
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 4);
	EXPECT_TRUE(callback->binaryPackets_[0] == std::string("\v\f\r\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"));
	EXPECT_TRUE(callback->binaryPackets_[1] == std::string("\x1\x2\x3\x4\x5\x6\a\b\t\n"));
	EXPECT_TRUE(callback->binaryPackets_[2] == std::string("\xf\n\v\f\r\xe\x1\xf"));
	EXPECT_TRUE(callback->binaryPackets_[3] == std::string("!!!"));
	EXPECT_TRUE(callback->textPackets_.size() == 4);
	EXPECT_TRUE(callback->textPackets_[0] == std::string("abcdefghijklmn!"));
	EXPECT_TRUE(callback->textPackets_[1] == std::string("opqrstuvw!"));
	EXPECT_TRUE(callback->textPackets_[2] == "Hello World!!");
	EXPECT_TRUE(callback->textPackets_[3] == std::string("!!!!!!!"));
}

TEST(ISS_TASK, empty_chunk) 
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive(nullptr, 0);
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.empty());
}

TEST(ISS_TASK, empty_binary)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("$\x00\x00\x00\x00", 5);
	}
	EXPECT_TRUE(callback->textPackets_.empty());
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == "");
}

TEST(ISS_TASK, empty_text)
{
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive("\r\n\r\n", 4);
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == "");
}

TEST(ISS_TASK, basic_text)
{
	std::string data = "simple text\r\n\r\n";
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive(&data[0], data.size());
	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == "simple text");
}

TEST(ISS_TASK, basic_binary)
{
	char data[] = "$" "\x03\x00\x00\x00" "abc";
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive(&data[0], std::size(data));
	}
	EXPECT_TRUE(callback->textPackets_.empty());
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == "abc");
}

TEST(ISS_TASK, basic_mixed)
{
	char mixed_data[] = "$" "\x03\x00\x00\x00" "abc" "text\r\n\r\n";
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive(&mixed_data[0], std::size(mixed_data));
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == "abc");
	EXPECT_TRUE(callback->textPackets_[0] == "text");
}

TEST(ISS_TASK, chunked_mixed)
{
	char mixed_data[] = "$" "\x03\x00\x00\x00" "abc" "text\r\n\r\n";
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		for (auto ch : mixed_data) {
			receiver.Receive(&ch, 1);
		}
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->binaryPackets_[0] == "abc");
	EXPECT_TRUE(callback->textPackets_[0] == "text");
}

TEST(ISS_TASK, complex_chunked_mixed)
{
	char data[] = "$" "\x06\x00\x00\x00" "binary"
		"text1\r\n\r\n"
		"text2\r\n\r\n"
		"$" "\x07\x00\x00\x00" "binary2"
		"text3\r\n\r\r\n\r\n\r\n"
		"padding";
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		for (auto i = 0u; i < std::size(data); i += 3) {
			receiver.Receive(data + i, 3);
		}
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 2);
	EXPECT_TRUE(callback->binaryPackets_[0] == "binary");
	EXPECT_TRUE(callback->binaryPackets_[1] == "binary2");
	EXPECT_TRUE(callback->textPackets_.size() == 3);
	EXPECT_TRUE(callback->textPackets_[0] == "text1");
	EXPECT_TRUE(callback->textPackets_[1] == "text2");
	EXPECT_TRUE(callback->textPackets_[2] == "text3\r\n\r");
}

TEST(ISS_TASK, binary_prefix_inside_text)
{
	std::string data = "text with $ in it\r\n\r\n";
	const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive(&data[0], data.size());

	}
	EXPECT_TRUE(callback->binaryPackets_.empty());
	EXPECT_TRUE(callback->textPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_[0] == "text with $ in it");
}

TEST(ISS_TASK, long_binary)
{
	std::vector<char> data{ *"$", 0x30, 0x20, 0x10, 0x00 };
	for (int i = 0; i < 0x102030; ++i) {
		data.push_back(i);
	}
		const std::shared_ptr<PacketCallback> callback(std::make_shared<PacketCallback>());
	{
		PacketReceiver receiver(callback);
		receiver.Receive(&data[0], std::size(data));
	}
	EXPECT_TRUE(callback->binaryPackets_.size() == 1);
	EXPECT_TRUE(callback->textPackets_.empty());
	auto& binary = callback->binaryPackets_[0];
	EXPECT_TRUE(binary.size() == 0x102030);
	EXPECT_TRUE(std::equal(begin(data) + 5, end(data), begin(binary), end(binary)));
}


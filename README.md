# iss_packet_parser
ISS test task

Написать класс с интерфейсом:

struct IReceiver
{
	virtual void Receive(const char* data, unsigned int size) = 0;
};

На вход которого (через интерфейс IReceiver) блоками различного размера непрерывно поступает поток данных.

Сам поток представляет собой последовательность пакетов двух типов (пакеты идут вперемешку):

1. Если пакет начинается с байта 0x24 (1 байт), то это "бинарный" пакет. За 0x24 следует размер данных пакета в байтах (четырёхбайтовое целое, little endian), а затем сами данные. "Бинарный" пакет нулевого размера возможен.

2. Иначе (если первый байт в пакете не 0x24) пакет текстовый. Текстовый пакет заканчивается на последовательность байт «\r\n\r\n».

Как только обнаружен конец пакета, необходимо данные пакета передать потребителю через callback-интерфейс:

struct ICallback
{
	virtual void BinaryPacket(const char* data, unsigned int size) = 0;
	virtual void TextPacket(const char* data, unsigned int size) = 0;
};

В обе функции за раз передаётся всё содержимое пакета целиком, без заголовков и «\r\n\r\n».

Данные одного пакета идут в потоке строго последовательно, данные разных пакетов не перемешиваются. Нет никакой привязки блока, поступающего на вход IReceiver к размерам и границам пакетов, на несколько блоков может разделиться в том числе размер бинарного пакета и терминатор текстового.
Класс должен быть как можно эффективнее при следующих условиях: средний поток, подаваемый на вход – 500 Mbit/s, средняя длина блока входных данных – 1024 байт.

В решении важна простота и читаемость.

The problem

You need to write an implementation of the following interface:

struct IReceiver {
    virtual void Receive(const char* data, unsigned int size) = 0;
    virtual ~Receive() = default;
};

The Receive member function is called continuously with two types of data: binary and text.

    The binary data always starts with byte 0x24, followed by a 4-byte integer, indicating the size of the packet. Then the packet body follows.

    The text data never starts with byte 0x24. No length is provided. The packet body always ends with two consecutive empty lines \r\n\r\n.

The important caveat is that a packet can span several Receive invocations. Also a single Receive invocation might be provided with several packets at once.

Each time a complete packet is received, the IReceiver descendant needs to invoke the right member function of the following callback class:

struct ICallback {
    virtual void BinaryPacket(const char* data, unsigned int size) = 0;
    virtual void TextPacket(const char* data, unsigned int size) = 0;
    virtual ~ICallback() = default;
};

The BinaryPacket and TextPacket functions should both receive a complete packet body, without the binary header or the two trailing empty lines.

The solution is judged by its performance, simplicity and readability. The language to use is C++.
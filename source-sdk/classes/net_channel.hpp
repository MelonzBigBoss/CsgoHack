#pragma once
class bf_write
{
public:
	bf_write() = default;

	// the current buffer
	unsigned long* pData;
	int				nDataBytes;
	int				nDataBits;

	// where we are in the buffer
	int				iCurrentBit;
};
class bf_read
{
public:
	std::uintptr_t uBaseAddress;
	std::uintptr_t uCurrentOffset;

	bf_read(std::uintptr_t uAddress) : uBaseAddress(uAddress), uCurrentOffset(0U) {}

	void SetOffset(std::uintptr_t uOffset)
	{
		uCurrentOffset = uOffset;
	}

	void Skip(std::uintptr_t uLength)
	{
		uCurrentOffset += uLength;
	}

	int ReadByte()
	{
		char dValue = *reinterpret_cast<char*>(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		return dValue;
	}

	bool ReadBool()
	{
		bool bValue = *reinterpret_cast<bool*>(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		return bValue;
	}

	const char* ReadString()
	{
		char szBuffer[256];
		char chLength = *reinterpret_cast<char*>(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		memcpy(szBuffer, reinterpret_cast<void*>(uBaseAddress + uCurrentOffset), chLength > 255 ? 255 : chLength);
		szBuffer[chLength > 255 ? 255 : chLength] = '\0';
		uCurrentOffset += chLength + 1;
		return szBuffer;
	}
};
class i_net_message
{
public:
	virtual					~i_net_message() { }
	virtual void			SetNetChannel(void* pNetChannel) = 0;
	virtual void			SetReliable(bool bState) = 0;
	virtual bool			Process() = 0;
	virtual	bool			ReadFromBuffer(bf_read& buffer) = 0;
	virtual	bool			WriteToBuffer(bf_write& buffer) = 0;
	virtual bool			IsReliable() const = 0;
	virtual int				GetType() const = 0;
	virtual int				GetGroup() const = 0;
	virtual const char* GetName() const = 0;
	virtual i_net_message* GetNetChannel() const = 0;
	virtual const char* ToString() const = 0;
	virtual std::size_t		GetSize() const = 0;
};
class i_net_channel {
public:
	std::byte	pad0[0x14];				//0x0000
	bool		bProcessingMessages;	//0x0014
	bool		should_delete;			//0x0015
	bool		bStopProcessing;		//0x0016
	std::byte	pad1[0x1];				//0x0017
	int			out_sequence_nr;			//0x0018 last send outgoing sequence number
	int			in_sequence_nr;			//0x001C last received incoming sequence number
	int			out_sequence_nr_ack;		//0x0020 last received acknowledge outgoing sequence number
	int			out_reliable_state;		//0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int			in_reliable_state;		//0x0028 state of incoming reliable data
	int			choked_packets;			//0x002C number of choked packets
	std::byte	pad2[0x414];			//0x0030

	int	SendDatagram(bf_write* pDatagram)
	{
		using VirtualFn = int(__thiscall*)(void*, bf_write*);
		return (*(VirtualFn**)(this))[46](this, pDatagram);
	}
};
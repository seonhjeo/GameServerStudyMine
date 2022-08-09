using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/* 리시브로 받은 데이터를 저장하는 클래스 */
namespace csharp_test_client
{
    class PacketBufferManager
    {
        int BufferSize = 0;
        int ReadPos = 0;        // 버퍼의 데이터 중 패킷으로 변환할 위치
        int WritePos = 0;       // 버퍼에 서버가 보낸 패킷을 복사할 위치

        int HeaderSize = 0;
        int MaxPacketSize = 0;
        byte[] PacketData;      // 리시브로 받은 바이너리 데이터
        byte[] PacketDataTemp;

        public bool Init(int size, int headerSize, int maxPacketSize)
        {
            if (size < (maxPacketSize * 2) || size < 1 || headerSize < 1 || maxPacketSize < 1)
            {
                return false;
            }

            BufferSize = size;
            PacketData = new byte[size];
            PacketDataTemp = new byte[size];
            HeaderSize = headerSize;
            MaxPacketSize = maxPacketSize;

            return true;
        }

        // 멤버변수 PacketData에 binary data를 복사하는 함수
        // 링 버퍼에 복사하게 됨
        public bool Write(byte[] data, int pos, int size)
        {
            if (data == null || (data.Length < (pos + size)))
            {
                return false;
            }

            var remainBufferSize = BufferSize - WritePos;

            if (remainBufferSize < size)
            {
                return false;
            }

            Buffer.BlockCopy(data, pos, PacketData, WritePos, size);
            WritePos += size;

            // 버퍼가 다 차면 맨 앞에서부터 다시 write하게끔 하기
            if (NextFree() == false)
            {
                BufferRelocate();
            }
            return true;
        }

        // binary data를 다시 패킷 구조체로 만들어주는 함수
        public ArraySegment<byte> Read()
        {
            var enableReadSize = WritePos - ReadPos;

            if (enableReadSize < HeaderSize)
            {
                return new ArraySegment<byte>();
            }

            var packetDataSize = BitConverter.ToInt16(PacketData, ReadPos);
            if (enableReadSize < packetDataSize)
            {
                return new ArraySegment<byte>();
            }

            var completePacketData = new ArraySegment<byte>(PacketData, ReadPos, packetDataSize);
            ReadPos += packetDataSize;
            return completePacketData;
        }

        // 현재 남은 공간이 패킷의 최대 크기보다 작으면 false 반환
        bool NextFree()
        {
            var enableWriteSize = BufferSize - WritePos;

            if (enableWriteSize < MaxPacketSize)
            {
                return false;
            }

            return true;
        }

        // 버퍼 재정렬
        void BufferRelocate()
        {
            var enableReadSize = WritePos - ReadPos;

            Buffer.BlockCopy(PacketData, ReadPos, PacketDataTemp, 0, enableReadSize);
            Buffer.BlockCopy(PacketDataTemp, 0, PacketData, 0, enableReadSize);

            ReadPos = 0;
            WritePos = enableReadSize;
        }
    }
}

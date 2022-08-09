using System;
using System.Net.Sockets;
using System.Net;

/* 네트워크 통신과 관련된 코드 */
// 동기 I/O 방식을 사용한 TCP통신
namespace csharp_test_client
{
    public class ClientSimpleTcp
    {
        public Socket Sock = null;   
        public string LatestErrorMsg;
        

        // 소켓연결 (클라이언트->서버)        
        public bool Connect(string ip, int port)
        {
            try
            {
                IPAddress serverIP = IPAddress.Parse(ip);
                int serverPort = port;

                Sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Sock.Connect(new IPEndPoint(serverIP, serverPort));

                if (Sock == null || Sock.Connected == false)
                {
                    return false;
                }

                return true;
            }
            catch (Exception ex)
            {
                LatestErrorMsg = ex.Message;
                return false;
            }
        }

        // 서버가 보내는 데이터를 수신하는 함수
        public Tuple<int,byte[]> Receive()
        {

            try
            {
                byte[] ReadBuffer = new byte[2048];
                // 동기 I/O방식이므로 블로킹이 걸림
                // 새로운 스레드에서 실행해 블로킹 방지
                var nRecv = Sock.Receive(ReadBuffer, 0, ReadBuffer.Length, SocketFlags.None);

                if (nRecv == 0)
                {
                    return null;
                }

                return Tuple.Create(nRecv,ReadBuffer);
            }
            catch (SocketException se)
            {
                LatestErrorMsg = se.Message;
            }

            return null;
        }

        // 스트림에 쓰기
        // 상황을 조절할 수 있기 때문에 스레드에 올릴 필요 없음
        // 해당 클라이언트에서는 패킷을 버퍼에 저장한 후 다른 스레드를 이용해 전송
        public void Send(byte[] sendData)
        {
            try
            {
                if (Sock != null && Sock.Connected) //연결상태 유무 확인
                {
                    Sock.Send(sendData, 0, sendData.Length, SocketFlags.None);
                }
                else
                {
                    LatestErrorMsg = "먼저 채팅서버에 접속하세요!";
                }
            }
            catch (SocketException se)
            {
                LatestErrorMsg = se.Message;
            }
        }

        //소켓과 스트림 닫기
        public void Close()
        {
            if (Sock != null && Sock.Connected)
            {
                //Sock.Shutdown(SocketShutdown.Both);
                Sock.Close();
            }
        }

        public bool IsConnected() { return (Sock != null && Sock.Connected) ? true : false; }
    }
}

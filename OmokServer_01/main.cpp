#include <iostream>
#include <thread>

#include "Server.h"

// main, packet, packetID, packetProcess, server, user, usermanager
int main()
{
	Server server;
	// 서버를 init할 때 네트워크도 init해줌
	server.Init();

	/* 람다 표현식 */
	// 해당 구문은 runThread로 이름지은 std::thread 클래스의 생성자에 `[&]() { server.Run(); }`이라는 람다 표현식을 넘겨준 것
	std::thread runThread( [&]() { server.Run(); } );
	
	// getchar함수는 키보드 입력을 받을 때까지 대기 상태
	std::cout << "press any key to exit..." << std::endl;
	getchar();


	server.Stop();
	// 쓰레드가 작업을 종료할 때까지 기다리는 함수
	runThread.join();

	return 0;
}
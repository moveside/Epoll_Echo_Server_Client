# Epoll_Echo_Server_Client





# 목표
* C++ / OOP
* 비동기 네트워크 I/O(서버는 리눅스기 때문에 EPOLL 사용)
* 멀티 스레딩
* 멀티 스레딩에서의 자원 동기화(Mutex 등등..)
* 프로토콜 정의
* 자료구조(STL,원형 버퍼 등등..)

## 1st Milestone
* Client-Server 접속
* Client-Server는 정해진 Protocol을 이용해 통신
* Client는 사용자로부터 입력받은 문자열을 Server로 전송
* Server는 Client로부터 전송된 문자열을 다시 Client로 전송
* Client는 Server로부터 전송된 문자열을 출력
##  2nd Milestone
* Server는 Client로부터 전송된 문자열을 자료 구조에 삽입
* Server는 Client로부터 전송된 문자열을 자료 구조에서 삭제
* Server는 현재 문자열 정보를 Client에 전송
## 3rd Milestone
* N개의 Client가 동시에 Server의 문자열 자료구조에 삽입 및 삭제 요청

# 계획

## 1st Milestone
- Server 구현 (3h)
- Client 구현 (3h)
- Client에서 Server에 문자열 전송 (3h)
- Server에서 Client에 문자열 전송 (3h)
## 2nd Milestone
- Client UI 구현 (6h)
- Server 자료구조 구현 (6h)
- Client 전송된 문자열을 Server의 자료구조에 삽입/삭제 (6h)
- Client 요청시 Server에서 Client에 현재 데이터 전송 (6h)
## 3rd Milestone
- 멀티 스레드 설정 및 구현 (18h)
- 다수의 클라이언트 요청 처리 (12h)












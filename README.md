# 장애인 주차 관리 시스템
## 개발 환경: Crosscompile, NFS
### HOST PC
* Ubuntu 12.04 LTS
* C++(OpenCV)
* C (Module Programming)
### Target Board
* 4x ARM Cortex-A9
* Linux Kernel Version: 3.0.35   
> <span style="color:orange">2020-05-26</span>
* 개발 계획서 작성
> <span style="color:orange">2020-06-04</span>
* Target보드에 opencv 포팅 완료 및 크로스 컴파일 확인
* Target보드 카메라 세팅 필요
* Raspberry Pi 와 Target 보드 소켓 통신 프로그래밍 준비

## IP
- WIFI : wookkl
- Raspberry pi(Server) : 172.20.10.2
- Achro-I.MX6Q(Client_Target) : 172.20.10.5
- Android Studio(Client_Phone)
##Routing Table
- Default Gateway : 172.20.10.1
- Server와 Client_Target은 같은 Network에서 같은 Default Gateway를 설정하여 통신.


 
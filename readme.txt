18년 1학기 성균관대학교 종합설계프로젝트(캡스톤 프로젝트)

------------------------------------------
Project   : ARTIK과 진동감지센서(SW1801)를 활용한 세탁기 현황 확인 시스템
Board     : ARTIK 520 & WeMos D1R2(ESP8266)
Edit Tool : notepad++ v7.5.6
Compiler  :
  ARTIK 520  : GCC & Python 2.7
  WeMos D1R2 : Arduino IDE 1.8.5 (http://eduino.kr/product/detail.html?product_no=434&cate_no=24&display_group=1 참조)
------------------------------------------
Project 목적
- 기숙사의 세탁실의 현황을 파악, 세탁 편의성 도모

Project 개요
 WeMos D1 R2
- 진동감지센서와 WeMos D1R2를 이용, 세탁기의 작동 상황을 확인하고 세탁기의 상태를 구분
- 현재 세탁기의 상태를 Socket Communication(TCP Client)을 이용해 ARTIK 520에 전송

 ARTIK 520
- WeMos D1 R2에서 Socket Communication을 이용해 세탁기의 상태를 수신
- 세탁기의 상태가 바뀌었으면 해당 정보를 ARTIK Cloud로 전송
- ARTIK Cloud의 데이터를 읽어 휴대폰 어플리케이션으로 현재 세탁기 가동 상태 확인

------------------------------------------

CODE
Send_state.py
  - for ARTIK
  - WeMos D1 R2 보드에서 세탁기 상태를 전송받아 ARTIK Cloud에 전송하는 코드

wifi_client_ver.180610.ino
  - for WeMos D1 R2
  - 진동 센서를 통해 세탁기의 진동값을 확인, 세탁기의 상태를 구분하고 ARTIK에 전송

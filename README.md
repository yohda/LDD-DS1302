# LDD-DS1302

- DS1302는 I2C 통신이 아니다. 3개의 핀을 통신을 하는데, 오히려 I2C보다 통신에 있어서 규칙이 까다로웠다. 하나의 핀이 양방향으로 쓰이는 핀들은 항시 데이터시트의 타이밍 다이어그램을 정확히 체크해야 한다.

- 사용 기술
1. Descriptor based GPIO
2. Pinctrl
3. Devicetree & overlay

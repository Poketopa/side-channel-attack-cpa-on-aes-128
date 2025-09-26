# AES 10라운드 CPA 기반 키 추출

## 소개

**AES의 10번째 라운드 키**를 **상관 전력 분석(Correlation Power Analysis, CPA)** 을 통해 추출하는 과정을 문서화 하였습니다.
실험은 **전력 트레이스 수집, 데이터 전처리, CPA 공격 수행, 키 스케줄 역연산을 통한 마스터 키 복원**으로 구성됩니다.

---

## 실험 환경

- **대상 알고리즘**: AES-128
- **전력 분석 기법**: Correlation Power Analysis (CPA)
- **캡처된 라운드**: 9라운드 AddRoundKey ~ 10라운드 최종 AddRoundKey
- **트리거 지점**:
  - 9라운드 AddRoundKey 시작
  - AES 암호화 종료
- **사용 장비 및 도구**:
  - **오실로스코프(Oscilloscope)**: 전력 소비 트레이스 수집
  - **아두이노(Arduino)**: AES 암호화 수행
  - **estraces**: 트레이스 저장 및 변환
  - **MATLAB**: 트레이스 시각화 및 상관 분석
  - **C 프로그래밍**: CPA 알고리즘 구현
  - **Python**: 키 스케줄 역연산 및 마스터 키 복원

---

## 저장소 구성

📂 **AES_CPA_Attack**  
 ├── 📂 **Assignment_1** # 1차 과제 – AES CPA 공격  
 │ ├── 📄 _AES_10-Round_CPA-Based_Key_Extraction_Report.pdf_  
 │  
 ├── 📂 **Assignment_2** # 2차 과제 – ARIA 마스킹  
 │ ├── 📄 _ARIA_specification.pdf_  
 │ ├── 📄 _ARIA_testVector.pdf_  
 │ ├── 📄 _Design_and_Implementation_of_Masking_Side-Channel_Countermeasure.pdf_  
 │ ├── 📝 _aria.c_  
 │ ├── 📝 _ariaMasking.c_  
 │  
 ├── 📂 **AES** # AES 관련 코드 구현  
 │ ├── 📝 _AES_CPA.c_ # C로 구현한 CPA 공격  
 │ ├── 📝 _AES_Decrypt.c_ # AES 복호화 코드  
 │ ├── 📝 _AES_FileEnc.c_ # 파일 암호화 처리  
 │ ├── 📝 _AES_masking.c_ # AES 마스킹 구현  
 │ ├── 📝 _AES_Optimization.c_ # 최적화된 AES 구현  
 │ ├── 📝 _AES_Optimization.h_ # AES 최적화 헤더 파일  
 │ ├── 📝 _AES.c_ # AES 암호화 코드  
 │ ├── 📝 _AES.h_ # AES 헤더 파일  
 │ ├── 📝 _CPA.c_ # CPA 알고리즘 구현  
 │ ├── 📝 _invCPA.c_ # 역 CPA 알고리즘 구현  
 │  
 ├── 📂 **trace** # 전력 트레이스 처리  
 │ ├── 📝 _arduino_AES.ino_ # 아두이노 AES 스크립트  
 │ ├── 📝 _Aligned.c_ # 트레이스 정렬 코드  
 │ ├── 📄 _ciphertext.txt_ # 암호문 데이터  
 │ ├── 🐍 _ETStoTRACES.py_ # ETS → 트레이스 변환 스크립트  
 │ ├── 🐍 _findPTCTKey.py_ # 평문·암호문 키 탐색 스크립트  
 │ ├── 📄 _key.txt_ # AES 키 데이터  
 │ ├── 📄 _plaintext.txt_ # 평문 데이터  
 │ ├── 📄 _read_trace.m_ # MATLAB용 트레이스 읽기 스크립트  
 │ ├── 📄 _SubChannel_Instruction.ipynb_ # 사이드 채널 분석 노트북  
 │  
 ├── 📄 _README.md_ # 프로젝트 설명 문서

---

## CPA 분석 과정

### 1. 전력 트레이스 수집

- AES 암호화를 수행하며 오실로스코프로 전력 트레이스를 수집하였습니다.
- 트리거는 **9라운드 AddRoundKey 시작**과 **AES 암호화 종료**에 설정하였습니다.

---

### 2. 데이터 전처리

- 수집한 `.ets` 전력 트레이스 파일을 **estraces**를 이용해 `.traces` 형식으로 변환하였습니다.
- 추출된 평문, 암호문, 전력 트레이스는 각각 `plaintext.txt`, `ciphertext.txt`, `traces.traces`로 저장하였습니다.

---

### 3. CPA 수행

- 공격 대상은 **AES 복호화 10라운드의 InvSBox 출력**입니다.
- CPA 알고리즘은 전력 트레이스와 **InvSBox(ciphertext ⊕ key_guess)** 의 해밍 가중치(Hamming Weight, HW)를 상관 분석합니다.
- 상관계수가 가장 높은 키 후보가 추출됩니다.

---

### 4. 키 스케줄 역연산

- 추출된 **10라운드 키**를 사용해 AES의 **마스터 키(master key)** 를 역연산으로 복원합니다.
- 역연산 과정에는 Python 스크립트를 사용하였습니다.

---

## 결과

- CPA를 통해 **AES 10라운드 키**를 성공적으로 추출하였습니다.
- **AES 키 스케줄 역연산**을 수행하여 **마스터 키**를 복원하였습니다.
- 상관계수 그래프를 통해 공격의 성공을 검증하였습니다.

---

### 전력 트레이스 시각화

<img width="500" alt="Image" src="https://github.com/user-attachments/assets/dbdbf2b9-4639-4271-b908-332cf0404111" />

---

### 상관계수 분석

<img width="500" alt="Image" src="https://github.com/user-attachments/assets/e214d1e0-58d2-41eb-8d26-d9018fba42f6" />

# AES 10-Round CPA-Based Key Extraction

## Introduction

This repository documents the process of extracting the **10th round key of AES** using **Correlation Power Analysis (CPA)**. The experiment involves **power trace collection, data preprocessing, CPA attack, and key schedule reversal to derive the master key**.

---

## Experiment Setup

- **Target Algorithm**: AES-128
- **Power Analysis Technique**: Correlation Power Analysis (CPA)
- **Captured Rounds**: 9th Round AddRoundKey ~ 10th Round Final AddRoundKey
- **Trigger Points**:
  - Start of 9th Round AddRoundKey
  - End of AES encryption
- **Equipment & Tools**:
  - **Oscilloscope**: Captures power consumption traces
  - **Arduino**: AES encryption execution
  - **estraces**: Used for trace storage and conversion
  - **MATLAB**: Trace visualization & correlation analysis
  - **C Programming**: CPA implementation
  - **Python**: Key expansion reversal to extract the master key

---

## Repository Structure

📂 **AES_CPA_Attack**  
 ├── 📂 **Assignment_1** # First assignment - AES CPA attack  
 │ ├── 📄 _AES_10-Round_CPA-Based_Key_Extraction_Report.pdf_  
 │  
 ├── 📂 **Assignment_2** # Second assignment - ARIA masking  
 │ ├── 📄 _ARIA_specification.pdf_  
 │ ├── 📄 _ARIA_testVector.pdf_  
 │ ├── 📄 _Design_and_Implementation_of_Masking_Side-Channel_Countermeasure.pdf_  
 │ ├── 📝 _aria.c_  
 │ ├── 📝 _ariaMasking.c_  
 │  
 ├── 📝 _arduino_AES.ino_ # Arduino script for AES execution  
 ├── 📝 _AES_CPA.c_ # CPA attack implementation in C  
 ├── 📝 _AES_Decrypt.c_ # AES decryption code  
 ├── 📝 _AES_FileEnc.c_ # AES file encryption handling  
 ├── 📝 _AES_masking.c_ # AES masking implementation  
 ├── 📝 _AES_Optimization.c_ # Optimized AES implementation  
 ├── 📝 _AES_Optimization.h_ # Header file for AES optimizations  
 ├── 📝 _AES.c_ # AES encryption implementation  
 ├── 📝 _AES.h_ # Header file for AES functions  
 ├── 📝 _CPA.c_ # CPA attack implementation  
 ├── 🐍 _ETStoTRACES.py_ # Python script to convert ETS to traces  
 ├── 🐍 _findPTCTKey.py_ # Script to find plaintext-ciphertext keys  
 ├── 📝 _invCPA.c_ # Inverse CPA implementation  
 ├── 📄 _read_trace.m_ # MATLAB script for trace reading

---

## CPA Analysis

### 1. Power Trace Collection

- Power traces were captured using an oscilloscope while performing AES encryption.
- Triggers were set at the **start of 9th round AddRoundKey** and **end of AES encryption**.

### 2. Data Preprocessing

- The collected `.ets` power trace files were converted into `.traces` format using **estraces**.
- Extracted plaintexts, ciphertexts, and power traces were stored as `plaintext.txt`, `ciphertext.txt`, and `traces.traces`.

### 3. CPA Execution

- The attack targets **InvSBox** outputs in the **10th round of AES decryption**.
- The CPA algorithm correlates power traces with **Hamming Weight (HW)** of **InvSBox(ciphertext ⊕ key_guess)**.
- The key candidate with the highest correlation was selected.

### 4. Key Schedule Reversal

- The extracted **10th round key** was used to compute the **AES master key** by reversing the key schedule.
- Python scripts were utilized for the reversal process.

---

## Results

- Successfully extracted the **10th round AES key** through CPA.
- Used **AES key expansion reversal** to derive the **original master key**.
- Correlation coefficient graphs confirmed the effectiveness of the attack.

### Power Trace Visualization

<img width="181" alt="Image" src="https://github.com/user-attachments/assets/dbdbf2b9-4639-4271-b908-332cf0404111" />

### Correlation Coefficient Analysis

<img width="260" alt="Image" src="https://github.com/user-attachments/assets/e214d1e0-58d2-41eb-8d26-d9018fba42f6" />

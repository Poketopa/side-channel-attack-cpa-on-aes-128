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

## Repository Structure

📂 AES_CPA_Attack ├── 📂 Assignment_1 # First assignment - AES CPA attack │ ├── AES_10-Round_CPA-Based_Key_Extraction_Report.pdf │ ├── 📂 Assignment_2 # Second assignment - ARIA masking │ ├── ARIA_specification.pdf │ ├── ARIA_testVector.pdf │ ├── aria.c │ ├── ariaMasking.c │ ├── Design_and_Implementation_of_Masking_Side-Channel_Countermeasure.pdf │ ├── .gitignore # Ignore unnecessary files │ ├── arduino_AES.ino # Arduino script for AES execution ├── AES_CPA.c # CPA attack implementation in C ├── AES_Decrypt.c # AES decryption code ├── AES_FileEnc.c # AES file encryption handling ├── AES_masking.c # AES masking implementation ├── AES_Optimization.c # Optimized AES implementation ├── AES_Optimization.h # Header file for AES optimizations ├── AES.c # AES encryption implementation ├── AES.h # Header file for AES functions │ ├── ciphertext.txt # Extracted ciphertexts ├── CPA.c # CPA attack implementation ├── ETStoTRACES.py # Python script to convert ETS to traces ├── findPTCTKey.py # Script to find plaintext-ciphertext keys ├── invCPA.c # Inverse CPA implementation ├── key.txt # Extracted key information ├── plaintext.txt # Extracted plaintexts │ ├── read_trace.m # MATLAB script for trace reading ├── SubChannel_Instruction.ipynb # Jupyter Notebook for subchannel analysis │ └── images/ # Folder for result images ├── power_trace.png # Example power trace ├── correlation_plot.png # Correlation coefficient graph


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
![Power Trace](images/power_trace.png)

### Correlation Coefficient Analysis
![Correlation Coefficient](images/correlation_plot.png)


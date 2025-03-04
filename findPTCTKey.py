import os
import estraces

def save_to_file(filename, data):
    """
    Save 16-byte hex strings to a file with specified formatting.
    Each entry in data is expected to be a string of 32 hex characters.
    """
    with open(filename, 'w', encoding='utf-8') as file:
        for entry in data:
            formatted = " ".join([entry[i:i+2] for i in range(0, len(entry), 2)])
            file.write(formatted + "\n")

# Define paths
ets_file_path = "/Users/lhs/Desktop/실습/trc.ets"
output_folder = "/Users/lhs/Desktop/실습"
plaintext_path = os.path.join(output_folder, "plaintext.txt")
ciphertext_path = os.path.join(output_folder, "ciphertext.txt")
key_path = os.path.join(output_folder, "key.txt")

# Read .ets file and extract data
ths = estraces.read_ths_from_ets_file(ets_file_path)

# Convert metadata to hex strings
plaintexts = ["".join(f"{byte:02x}" for byte in plain) for plain in ths.plaintext]
ciphertexts = ["".join(f"{byte:02x}" for byte in cipher) for cipher in ths.ciphertext]
keys = ["".join(f"{byte:02x}" for byte in key) for key in ths.key]

# Save data to files
save_to_file(plaintext_path, plaintexts)
save_to_file(ciphertext_path, ciphertexts)
save_to_file(key_path, keys)

# Print saved file locations
print(f"Files saved:\nPlaintext: {plaintext_path}\nCiphertext: {ciphertext_path}\nKey: {key_path}")
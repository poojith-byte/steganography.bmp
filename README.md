# 🖼️ LSB Steganography (C Project)

## 📌 Description

This project implements **Least Significant Bit (LSB) Steganography** using the **C programming language**.
It allows users to **hide a secret text file inside a BMP image** and later **extract the hidden data** from the stego image.

The project works only with **`.bmp` image files** to ensure uncompressed pixel data.

---

## 📂 Project Files

```
STEGANO_25036A+39/
│
├── main.c          # Main driver program
├── encode.c        # Encoding logic
├── decode.c        # Decoding logic
├── encode.h        # Encode function declarations
├── decode.h        # Decode function declarations
├── common.h        # Common utilities
├── types.h         # Custom data types
│
├── beautiful.bmp   # Input image file
├── stego.bmp       # Output stego image
├── secret.txt      # Secret text file
├── stego.txt       # Decoded output text (default)
├── README.md
```

---

## ⚙️ Compilation

Compile **all `.c` files together** using GCC:

```bash
gcc *.c
```

This will generate an executable file:

```bash
a.out
```

---

## ▶️ How to Run

### 🔐 Encoding (Hide Secret Data)

```bash
./a.out -e beautiful.bmp stego.bmp
```

### 🔹 Explanation:

* `-e` → Encode option
* `beautiful.bmp` → Input image file (**must be .bmp**)
* `stego.bmp` → Output image with hidden data (**optional**)

  * If not provided, input image name will be reused
* Secret data is read from `secret.txt`

---

### 🔓 Decoding (Extract Secret Data)

```bash
./a.out -d stego.bmp out.txt
```

### 🔹 Explanation:

* `-d` → Decode option
* `stego.bmp` → Stego image containing hidden data
* `out.txt` → Output text file (**optional**)

  * If not provided, output defaults to `stego.txt`
  * Output file **must be `.txt`**

---

## 📌 Important Notes

* Only **BMP image format** is supported
* Output text file must be **`.txt`**
* Large secret files may affect image quality
* Best suited for **educational purposes**

---

## 🧪 Example Workflow

```bash
gcc *.c
./a.out -e beautiful.bmp stego.bmp
./a.out -d stego.bmp decoded.txt
```

---

## 🚀 Future Enhancements

* Add encryption before embedding data
* Support PNG/JPEG formats
* Add password protection
* Create a GUI interface

---

## 👨‍💻 Author

**Poojith M**
-Project – LSB Steganography in C

---

## 📜 License

This project is intended for **academic use**.

---

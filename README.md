# TerminalFileStream
A program that allows sending file from the terminal.

When it starts, It will show the numbers of bytes to send; and then the percentage after the 1st percent is sent.

---
**Compilation**

use g++ or another compiler on main.cpp

---

**Example usage:**

<code>TerminalFileStream receive 9999 > output.bin</code>

<code>TerminalFileStream send 192.168.0.8 9999 input.bin</code>

;



<code>TerminalFileStream receive 9999 > /dev/sdb</code>

<code>TerminalFileStream send 192.168.0.8 9999 input.img</code>

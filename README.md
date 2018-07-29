# QMX
QMX compression library

[![Build Status](https://travis-ci.org/amallia/QMX.svg?branch=master)](https://travis-ci.org/amallia/QMX)

QMX compression improved (smaller and faster to decode)
Trotman & Lin  describe several improvements to the QMX codex in:
> A. Trotman, J. Lin (2016), In Vacuo and In Situ Evaluation of SIMD Codecs, Proceedings of The 21st Australasian Document Computing Symposium (ADCS 2016

including removal of the vbyte encoded length from the end of the encoded sequence.  This version of QMX is the original QMX with that improvement added, but none of the other imprivements suggested by Trotman & Lin.  This makes the encoded sequence smaller, and faster to decode, than any of the other alrernatives suggested.  It does not include the code to prevent read and write overruns from the encoded string and into the decode buffer.  To account for overwrites make sure the decode-into buffer is at least 256 integers larger than required.  To prevent over-reads from the encoded string make sure that that string is at least 16 bytes longer than needed.

At the request of Matthias Petri (University of Melbourne), the code no longer requires SIMD-word alignment to decode (the read and write instructions have been changed from aligned to unaligned since Intel made them faster).

For details on the original QMX encoding see:
> A. Trotman (2014), Compression, SIMD, and Postings Lists, Proceedings of the 19th Australasian Document Computing Symposium (ADCS 2014)

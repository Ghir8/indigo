#ifndef PTP_CAMERA_MODEL_H
#define PTP_CAMERA_MODEL_H

/* Created by tools/make_dslr_table.py

   WARNING! All changes made in this file will be lost! */

static ptp_camera_model CAMERA[] = {
  { 0x045e, 0x0a00, "Microsoft Lumia", 0, 0, 0, 0 },
  { 0x05ac, 0x12a8, "Apple iPhone", 0, 0, 0, 0 },
  { CANON_VID, 0x3110, "Canon EOS 400D", 0, 3888, 2592, 5.7 },
  { CANON_VID, 0x3113, "Canon EOS 30D", 0, 3504, 2336, 6.4 },
  { CANON_VID, 0x3145, "Canon EOS 450D", ptp_flag_lv, 4272, 2848, 5.19 },
  { CANON_VID, 0x3146, "Canon EOS 40D", ptp_flag_lv, 3888, 2592, 5.7 },
  { CANON_VID, 0x3147, "Canon EOS 1D Mark III", 0, 3888, 2592, 7.23 },
  { CANON_VID, 0x317b, "Canon EOS 1000D", ptp_flag_lv, 3888, 2592, 5.7 },
  { CANON_VID, 0x3199, "Canon EOS 5D Mark II", ptp_flag_lv, 5616, 3744, 6.41 },
  { CANON_VID, 0x319a, "Canon EOS 7D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x319b, "Canon EOS 50D", ptp_flag_lv, 4752, 3168, 4.69 },
  { CANON_VID, 0x31cf, "Canon EOS 500D", ptp_flag_lv, 4752, 3168, 4.69 },
  { CANON_VID, 0x31d0, "Canon EOS 1D Mark IV", ptp_flag_lv, 4896, 3264, 5.69 },
  { CANON_VID, 0x31ea, "Canon EOS 550D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x3215, "Canon EOS 60D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x3217, "Canon EOS 1100D", ptp_flag_lv, 4272, 2848, 5.19 },
  { CANON_VID, 0x3218, "Canon EOS 600D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x3219, "Canon EOS 1D X", ptp_flag_lv, 5184, 3456, 6.94 },
  { CANON_VID, 0x323a, "Canon EOS 5D Mark III", ptp_flag_lv, 5760, 3840, 6.25 },
  { CANON_VID, 0x323b, "Canon EOS 650D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x3250, "Canon EOS 6D", ptp_flag_lv, 5472, 3648, 6.54 },
  { CANON_VID, 0x3252, "Canon EOS 1D C", ptp_flag_lv, 5184, 3456, 6.94 },
  { CANON_VID, 0x3253, "Canon EOS 70D", ptp_flag_lv, 5472, 3648, 4.11 },
  { CANON_VID, 0x326f, "Canon EOS 7D Mark II", ptp_flag_lv, 5472, 3648, 4.1 },
  { CANON_VID, 0x3270, "Canon EOS 100D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x3272, "Canon EOS 700D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x3273, "Canon EOS M2", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x327f, "Canon EOS 1200D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x3280, "Canon EOS 760D", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x3281, "Canon EOS 5D Mark IV", ptp_flag_lv, 6720, 4480, 5.35 },
  { CANON_VID, 0x3292, "Canon EOS 1D X Mark II", ptp_flag_lv, 5472, 3648, 6.54 },
  { CANON_VID, 0x3294, "Canon EOS 80D", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x3295, "Canon EOS 5DS", ptp_flag_lv, 8688, 5792, 4.14 },
  { CANON_VID, 0x3299, "Canon EOS M3", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32a0, "Canon EOS M10", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x32a1, "Canon EOS 750D", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32af, "Canon EOS 5DSR", ptp_flag_lv, 8688, 5792, 4.14 },
  { CANON_VID, 0x32b4, "Canon EOS 1300D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x32bb, "Canon EOS M5", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32c5, "Canon EOS M6", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32c9, "Canon EOS 800D", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32ca, "Canon EOS 6D Mark II", ptp_flag_lv, 6240, 4160, 5.7 },
  { CANON_VID, 0x32cb, "Canon EOS 77D", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32cc, "Canon EOS 200D", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32d1, "Canon EOS M100", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32d2, "Canon EOS M50", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32d9, "Canon EOS 4000D", ptp_flag_lv, 5184, 3456, 4.3 },
  { CANON_VID, 0x32da, "Canon EOS R", ptp_flag_lv, 6720, 4480, 5.35 },
  { CANON_VID, 0x32e1, "Canon EOS 2000D", ptp_flag_lv, 6000, 4000, 3.71 },
  { CANON_VID, 0x32ea, "Canon EOS 90D", ptp_flag_lv, 6960, 4640, 3.2 },
  { NIKON_VID, 0x0406, "Nikon D70", 0, 3008, 2000, 7.87 },
  { NIKON_VID, 0x0408, "Nikon D2X", 0, 4288, 2848, 5.52 },
  { NIKON_VID, 0x040a, "Nikon D50", 0, 3008, 2000, 7.87 },
  { NIKON_VID, 0x040c, "Nikon D2HS", 0, 2464, 1632, 9.45 },
  { NIKON_VID, 0x040e, "Nikon D70S", 0, 3008, 2000, 7.87 },
  { NIKON_VID, 0x0410, "Nikon D200", 0, 3872, 2592, 6.09 },
  { NIKON_VID, 0x0412, "Nikon D80", 0, 3872, 2592, 6.09 },
  { NIKON_VID, 0x0414, "Nikon D40", 0, 3008, 2000, 7.87 },
  { NIKON_VID, 0x0416, "Nikon D2XS", 0, 4288, 2848, 5.52 },
  { NIKON_VID, 0x0418, "Nikon D40X", 0, 3872, 2592, 6.09 },
  { NIKON_VID, 0x041a, "Nikon D300", 0, 4288, 2848, 5.50 },
  { NIKON_VID, 0x041c, "Nikon D3", 0, 4256, 2832, 8.45 },
  { NIKON_VID, 0x041e, "Nikon D60", 0, 3872, 2592, 6.09 },
  { NIKON_VID, 0x0420, "Nikon D3X", ptp_flag_lv, 6048, 4032, 5.95 },
  { NIKON_VID, 0x0421, "Nikon D90", ptp_flag_lv, 4288, 2848, 5.50 },
  { NIKON_VID, 0x0422, "Nikon D700", ptp_flag_lv, 4256, 2832, 8.45 },
  { NIKON_VID, 0x0423, "Nikon D5000", ptp_flag_lv, 4288, 2848, 5.50 },
  { NIKON_VID, 0x0424, "Nikon D3000", 0, 3872, 2592, 6.09 },
  { NIKON_VID, 0x0425, "Nikon D300S", ptp_flag_lv, 4288, 2848, 5.50 },
  { NIKON_VID, 0x0426, "Nikon D3S", ptp_flag_lv, 4256, 2832, 8.45 },
  { NIKON_VID, 0x0427, "Nikon D3100", ptp_flag_lv, 4608, 3072, 5.0 },
  { NIKON_VID, 0x0428, "Nikon D7000", ptp_flag_lv, 4928, 3264, 4.78 },
  { NIKON_VID, 0x0429, "Nikon D5100", ptp_flag_lv, 4928, 3264, 4.78 },
  { NIKON_VID, 0x042a, "Nikon D800", ptp_flag_lv, 7360, 4912, 4.88 },
  { NIKON_VID, 0x042b, "Nikon D4", ptp_flag_lv, 4928, 3280, 7.30 },
  { NIKON_VID, 0x042c, "Nikon D3200", ptp_flag_lv, 6016, 4000, 3.92 },
  { NIKON_VID, 0x042d, "Nikon D600", ptp_flag_lv, 6016, 4016, 5.95 },
  { NIKON_VID, 0x042e, "Nikon D800E", ptp_flag_lv, 7360, 4912, 4.88 },
  { NIKON_VID, 0x042f, "Nikon D5200", ptp_flag_lv, 6000, 4000, 3.92 },
  { NIKON_VID, 0x0430, "Nikon D7100", ptp_flag_lv, 6000, 4000, 3.92 },
  { NIKON_VID, 0x0431, "Nikon D5300", ptp_flag_lv, 6000, 4000, 3.92 },
  { NIKON_VID, 0x0432, "Nikon Df", ptp_flag_lv, 4928, 3280, 7.3 },
  { NIKON_VID, 0x0433, "Nikon D3300", ptp_flag_lv, 6000, 4000, 3.92 },
  { NIKON_VID, 0x0434, "Nikon D610", ptp_flag_lv, 6016, 4016, 5.97 },
  { NIKON_VID, 0x0435, "Nikon D4S", ptp_flag_lv, 4928, 3280, 7.30 },
  { NIKON_VID, 0x0436, "Nikon D810", ptp_flag_lv, 7360, 4912, 4.88 },
  { NIKON_VID, 0x0437, "Nikon D750", ptp_flag_lv, 6016, 4016, 5.97 },
  { NIKON_VID, 0x0438, "Nikon D5500", ptp_flag_lv, 6000, 4000, 3.92 },
  { NIKON_VID, 0x0439, "Nikon D7200", ptp_flag_lv, 6000, 4000, 3.92 },
  { NIKON_VID, 0x043a, "Nikon D5", ptp_flag_lv, 5568, 3712, 6.40 },
  { NIKON_VID, 0x043b, "Nikon D810A", ptp_flag_lv, 7360, 4912, 4.88 },
  { NIKON_VID, 0x043c, "Nikon D500", ptp_flag_lv, 5568, 3712, 4.23 },
  { NIKON_VID, 0x043d, "Nikon D3400", ptp_flag_lv, 6000, 4000, 3.92 },
  { NIKON_VID, 0x043f, "Nikon D5600", ptp_flag_lv, 6000, 4000, 3.92 },
  { NIKON_VID, 0x0440, "Nikon D7500", ptp_flag_lv, 5568, 3712, 4.22 },
  { NIKON_VID, 0x0441, "Nikon D850", ptp_flag_lv, 8256, 5504, 4.34 },
  { NIKON_VID, 0x0442, "Nikon Z 7", ptp_flag_lv, 8256, 5504, 4.34 },
  { NIKON_VID, 0x0443, "Nikon Z 6", ptp_flag_lv, 6048, 4024, 5.9 },
  { NIKON_VID, 0x0445, "Nikon D3500", ptp_flag_lv, 6000, 4000, 3.92 },
  { SONY_VID, 0x079b, "Sony Alpha A68", ptp_flag_lv, 6000, 4000, 3.92 },
  { SONY_VID, 0x079c, "Sony Alpha A6300", ptp_flag_lv, 6000, 4000, 3.92 },
  { SONY_VID, 0x079e, "Sony Alpha A99II", ptp_flag_lv, 7952, 5304, 4.51 },
  { SONY_VID, 0x07a4, "Sony Alpha A6500", ptp_flag_lv, 6000, 4000, 3.92 },
  { SONY_VID, 0x07c6, "Sony Alpha A5000", 0, 5456, 3632, 4.25 },
  { SONY_VID, 0x094c, "Sony Alpha A7", ptp_flag_lv, 6000, 4000, 5.98 },
  { SONY_VID, 0x094d, "Sony Alpha A7R", ptp_flag_lv, 7360, 4912, 4.9 },
  { SONY_VID, 0x094e, "Sony Alpha A6000", ptp_flag_lv, 6000, 4000, 3.92 },
  { SONY_VID, 0x0953, "Sony Alpha A77M2", ptp_flag_lv, 6000, 4000, 3.92 },
  { SONY_VID, 0x0954, "Sony Alpha A7S", ptp_flag_lv, 4240, 2832, 8.4 },
  { SONY_VID, 0x0957, "Sony Alpha A5100", ptp_flag_lv, 6000, 4000, 3.92 },
  { SONY_VID, 0x096f, "Sony Alpha A7III", ptp_flag_lv, 6000, 4000, 5.98 },
  { SONY_VID, 0x0a6a, "Sony Alpha 7II", ptp_flag_lv, 6000, 4000, 5.98 },
  { SONY_VID, 0x0a6b, "Sony Alpha A7RII", ptp_flag_lv, 7952, 5304, 4.5 },
  { SONY_VID, 0x0a71, "Sony Alpha A7SII", 0, 4240, 2832, 8.39 },
  { SONY_VID, 0x0c2a, "Sony Alpha A9", ptp_flag_lv, 6000, 4000, 5.98 },
  { SONY_VID, 0x0c33, "Sony Alpha A7RIII", ptp_flag_lv, 7952, 5304, 4.5 },
  { SONY_VID, 0x0c34, "Sony Alpha A7III", ptp_flag_lv, 6000, 4000, 5.98 },
  { 0 },
};

#endif				/* PTP_CAMERA_MODEL_H */

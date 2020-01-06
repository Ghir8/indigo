#ifndef DSLR_MODEL_INFO_H
#define DSLR_MODEL_INFO_H

/* Created by tools/make_dslr_table.py

   WARNING! All changes made in this file will be lost! */

static struct dslr_model_info {
  const char	*name;
  int		 width, height;
  float		 pixel_size;
} dslr_model_info[] = {
  { "CANON EOS RP", 6240, 4160, 5.75 },
  { "CANON EOS REBEL XTI", 3888, 2592, 5.7 },
  { "CANON EOS REBEL XT", 3456, 2304, 6.4 },
  { "CANON EOS REBEL XSI", 4272, 2848, 5.19 },
  { "CANON EOS REBEL XS", 3888, 2592, 5.7 },
  { "CANON EOS REBEL T7I", 6000, 4000, 3.71 },
  { "CANON EOS REBEL T7", 6000, 4000, 3.71 },
  { "CANON EOS REBEL T6S", 6000, 4000, 3.71 },
  { "CANON EOS REBEL T6I", 6000, 4000, 3.71 },
  { "CANON EOS REBEL T6", 5184, 3456, 4.3 },
  { "CANON EOS REBEL T5I", 5184, 3456, 4.3 },
  { "CANON EOS REBEL T5", 5184, 3456, 4.3 },
  { "CANON EOS REBEL T4I", 5184, 3456, 4.3 },
  { "CANON EOS REBEL T3I", 5184, 3456, 4.3 },
  { "CANON EOS REBEL T3", 4272, 2848, 5.19 },
  { "CANON EOS REBEL T2I", 5184, 3456, 4.3 },
  { "CANON EOS REBEL T1I", 4752, 3168, 4.69 },
  { "CANON EOS REBEL T100", 5184, 3456, 4.3 },
  { "CANON EOS REBEL SL2", 6000, 4000, 3.71 },
  { "CANON EOS REBEL SL1", 5184, 3456, 4.3 },
  { "CANON EOS R", 6720, 4480, 5.35 },
  { "CANON EOS M6", 6000, 4000, 3.71 },
  { "CANON EOS M50", 6000, 4000, 3.71 },
  { "CANON EOS M5", 6000, 4000, 3.71 },
  { "CANON EOS M3", 6000, 4000, 3.71 },
  { "CANON EOS M2", 5184, 3456, 4.3 },
  { "CANON EOS M100", 6000, 4000, 3.71 },
  { "CANON EOS M10", 5184, 3456, 4.3 },
  { "CANON EOS KISS X9I", 6000, 4000, 3.71 },
  { "CANON EOS KISS X90", 6000, 4000, 3.71 },
  { "CANON EOS KISS X9", 6000, 4000, 3.71 },
  { "CANON EOS KISS X8I", 6000, 4000, 3.71 },
  { "CANON EOS KISS X80", 5184, 3456, 4.3 },
  { "CANON EOS KISS X7I", 5184, 3456, 4.3 },
  { "CANON EOS KISS X70", 5184, 3456, 4.3 },
  { "CANON EOS KISS X7", 5184, 3456, 4.3 },
  { "CANON EOS KISS X6I", 5184, 3456, 4.3 },
  { "CANON EOS KISS X50", 4272, 2848, 5.19 },
  { "CANON EOS KISS X5", 5184, 3456, 4.3 },
  { "CANON EOS KISS X4", 5184, 3456, 4.3 },
  { "CANON EOS KISS X3", 4752, 3168, 4.69 },
  { "CANON EOS KISS X2", 4272, 2848, 5.19 },
  { "CANON EOS KISS X", 3888, 2592, 5.7 },
  { "CANON EOS KISS N", 3456, 2304, 6.4 },
  { "CANON EOS KISS M", 6000, 4000, 3.71 },
  { "CANON EOS KISS F", 3888, 2592, 5.7 },
  { "CANON EOS 90D", 6960, 4640, 3.2 },
  { "CANON EOS 9000D", 6000, 4000, 3.71 },
  { "CANON EOS 80D", 6000, 4000, 3.71 },
  { "CANON EOS 800D", 6000, 4000, 3.71 },
  { "CANON EOS 8000D", 6000, 4000, 3.71 },
  { "CANON EOS 7D MARK II", 5472, 3648, 4.1 },
  { "CANON EOS 7D", 5184, 3456, 4.3 },
  { "CANON EOS 77D", 6000, 4000, 3.71 },
  { "CANON EOS 77D", 6000, 4000, 3.71 },
  { "CANON EOS 760D", 6000, 4000, 3.71 },
  { "CANON EOS 750D", 6000, 4000, 3.71 },
  { "CANON EOS 70D", 5472, 3648, 4.11 },
  { "CANON EOS 700D", 5184, 3456, 4.3 },
  { "CANON EOS 6D MARK II", 6240, 4160, 5.7 },
  { "CANON EOS 6D", 5472, 3648, 6.54 },
  { "CANON EOS 650D", 5184, 3456, 4.3 },
  { "CANON EOS 60D", 5184, 3456, 4.3 },
  { "CANON EOS 600D", 5184, 3456, 4.3 },
  { "CANON EOS 5DSR", 8688, 5792, 4.14 },
  { "CANON EOS 5DS", 8688, 5792, 4.14 },
  { "CANON EOS 5D MARK IV", 6720, 4480, 5.35 },
  { "CANON EOS 5D MARK III", 5760, 3840, 6.25 },
  { "CANON EOS 5D MARK II", 5616, 3744, 6.41 },
  { "CANON EOS 5D", 4368, 2912, 8.2 },
  { "CANON EOS 550D", 5184, 3456, 4.3 },
  { "CANON EOS 50D", 4752, 3168, 4.69 },
  { "CANON EOS 500D", 4752, 3168, 4.69 },
  { "CANON EOS 450D", 4272, 2848, 5.19 },
  { "CANON EOS 40D", 3888, 2592, 5.7 },
  { "CANON EOS 400D", 3888, 2592, 5.7 },
  { "CANON EOS 4000D", 5184, 3456, 4.3 },
  { "CANON EOS 350D", 3456, 2304, 6.4 },
  { "CANON EOS 30D", 3504, 2336, 6.4 },
  { "CANON EOS 20DA", 3504, 2336, 6.4 },
  { "CANON EOS 20D", 3504, 2336, 6.4 },
  { "CANON EOS 200D", 6000, 4000, 3.71 },
  { "CANON EOS 2000D", 6000, 4000, 3.71 },
  { "CANON EOS 1DS MARK III", 5616, 3744, 6.41 },
  { "CANON EOS 1D X MARK II", 5472, 3648, 6.54 },
  { "CANON EOS 1D X", 5184, 3456, 6.94 },
  { "CANON EOS 1D MARK IV", 4896, 3264, 5.69 },
  { "CANON EOS 1D MARK III", 3888, 2592, 7.23 },
  { "CANON EOS 1D C", 5184, 3456, 6.94 },
  { "CANON EOS 1300D", 5184, 3456, 4.3 },
  { "CANON EOS 1200D", 5184, 3456, 4.3 },
  { "CANON EOS 1100D", 4272, 2848, 5.19 },
  { "CANON EOS 100D", 5184, 3456, 4.3 },
  { "CANON EOS 1000D", 3888, 2592, 5.7 },
  { "NIKON Z 7", 8256, 5504, 4.34 },
  { "NIKON Z 6", 6048, 4024, 5.9 },
  { "NIKON DF", 4928, 3280, 7.3 },
  { "NIKON D90", 4288, 2848, 5.50 },
  { "NIKON D850", 8256, 5504, 4.34 },
  { "NIKON D810A", 7360, 4912, 4.88 },
  { "NIKON D810", 7360, 4912, 4.88 },
  { "NIKON D800E", 7360, 4912, 4.88 },
  { "NIKON D800", 7360, 4912, 4.88 },
  { "NIKON D80", 3872, 2592, 6.09 },
  { "NIKON D7500", 5568, 3712, 4.22 },
  { "NIKON D750", 6016, 4016, 5.97 },
  { "NIKON D7200", 6000, 4000, 3.92 },
  { "NIKON D7100", 6000, 4000, 3.92 },
  { "NIKON D70S", 3008, 2000, 7.87 },
  { "NIKON D7000", 4928, 3264, 4.78 },
  { "NIKON D700", 4256, 2832, 8.45 },
  { "NIKON D70", 3008, 2000, 7.87 },
  { "NIKON D610", 6016, 4016, 5.97 },
  { "NIKON D600", 6016, 4016, 5.95 },
  { "NIKON D60", 3872, 2592, 6.09 },
  { "NIKON D5600", 6000, 4000, 3.92 },
  { "NIKON D5500", 6000, 4000, 3.92 },
  { "NIKON D5300", 6000, 4000, 3.92 },
  { "NIKON D5200", 6000, 4000, 3.92 },
  { "NIKON D5100", 4928, 3264, 4.78 },
  { "NIKON D5000", 4288, 2848, 5.50 },
  { "NIKON D500", 5568, 3712, 4.23 },
  { "NIKON D50", 3008, 2000, 7.87 },
  { "NIKON D5", 5568, 3712, 6.40 },
  { "NIKON D4S", 4928, 3280, 7.30 },
  { "NIKON D40X", 3872, 2592, 6.09 },
  { "NIKON D40", 3008, 2000, 7.87 },
  { "NIKON D4", 4928, 3280, 7.30 },
  { "NIKON D3X", 6048, 4032, 5.95 },
  { "NIKON D3S", 4256, 2832, 8.45 },
  { "NIKON D3500", 6000, 4000, 3.92 },
  { "NIKON D3400", 6000, 4000, 3.92 },
  { "NIKON D3300", 6000, 4000, 3.92 },
  { "NIKON D3200", 6016, 4000, 3.92 },
  { "NIKON D3100", 4608, 3072, 5.0 },
  { "NIKON D300S", 4288, 2848, 5.50 },
  { "NIKON D3000", 3872, 2592, 6.09 },
  { "NIKON D300", 4288, 2848, 5.50 },
  { "NIKON D3", 4256, 2832, 8.45 },
  { "NIKON D2XS", 4288, 2848, 5.52 },
  { "NIKON D2X", 4288, 2848, 5.52 },
  { "NIKON D2HS", 2464, 1632, 9.45 },
  { "NIKON D200", 3872, 2592, 6.09 },
  { "SONY ALPHA A99II", 7952, 5304, 4.51 },
  { "SONY ALPHA A9", 6000, 4000, 5.98 },
  { "SONY ALPHA A7SII", 4240, 2832, 8.39 },
  { "SONY ALPHA A7S", 4240, 2832, 8.4 },
  { "SONY ALPHA A7RIII", 7952, 5304, 4.5 },
  { "SONY ALPHA A7RII", 7952, 5304, 4.5 },
  { "SONY ALPHA A7R", 7360, 4912, 4.9 },
  { "SONY ALPHA A7III", 6000, 4000, 5.98 },
  { "SONY ALPHA A7III", 6000, 4000, 5.98 },
  { "SONY ALPHA A77M2", 6000, 4000, 3.92 },
  { "SONY ALPHA A7", 6000, 4000, 5.98 },
  { "SONY ALPHA A68", 6000, 4000, 3.92 },
  { "SONY ALPHA A6500", 6000, 4000, 3.92 },
  { "SONY ALPHA A6300", 6000, 4000, 3.92 },
  { "SONY ALPHA A6000", 6000, 4000, 3.92 },
  { "SONY ALPHA A5100", 6000, 4000, 3.92 },
  { "SONY ALPHA A5000", 5456, 3632, 4.25 },
  { "SONY ALPHA 7II", 6000, 4000, 5.98 },
  { NULL, 0, 0, 0 },
};

#endif				/* DSLR_MODEL_INFO_H */

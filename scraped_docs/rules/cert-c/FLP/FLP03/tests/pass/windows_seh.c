void fp_usingSEH(void) {
  /* ... */
  double a = 1e-40, b, c = 0.1;
  float x = 0, y;
  unsigned int rv ;

  unmask_fpsr();

  _try {
    /* Store into y is inexact and underflows: */
    y = a;

    /* Divide-by-zero operation */
    b = y / x;

    /* Inexact */
    c = sin(30) * a;
  }

  _except (_fpieee_flt(
             GetExceptionCode(),
             GetExceptionInformation(),
             fpieee_handler)) {
  {
  printf ("fpieee_handler: EXCEPTION_EXECUTE_HANDLER");
  }

  /* ... */
}

void unmask_fpsr(void) {
  unsigned int u;
  unsigned int control_word;
  _controlfp_s(&control_word, 0, 0);
  u = control_word & ~(_EM_INVALID
                     | _EM_DENORMAL
                     | _EM_ZERODIVIDE
                     | _EM_OVERFLOW
                     | _EM_UNDERFLOW
                     | _EM_INEXACT);
  _controlfp_s( &control_word, u, _MCW_EM);
  return ;
}

int fpieee_handler(_FPIEEE_RECORD *ieee) {
  /* ... */

  switch (ieee->RoundingMode) {
    case _FpRoundNearest:
      /* ... */
      break;

      /*
       * Other RMs include _FpRoundMinusInfinity,
       * _FpRoundPlusInfinity, _FpRoundChopped.
       */

      /* ... */
    }

  switch (ieee->Precision) {
    case _FpPrecision24:
      /* ... */
      break;

      /* Other Ps include _FpPrecision53 */
      /* ... */
    }

   switch (ieee->Operation) {
     case _FpCodeAdd:
       /* ... */
       break;

       /* 
        * Other Ops include _FpCodeSubtract, _FpCodeMultiply,
        * _FpCodeDivide, _FpCodeSquareRoot, _FpCodeCompare,
        * _FpCodeConvert, _FpCodeConvertTrunc.
        */
       /* ... */
    }

  /* 
   * Process the bitmap ieee->Cause.
   * Process the bitmap ieee->Enable.
   * Process the bitmap ieee->Status.
   * Process the Operand ieee->Operand1, 
   * evaluate format and Value.
   * Process the Operand ieee->Operand2, 
   * evaluate format and Value.
   * Process the Result ieee->Result, 
   * evaluate format and Value.
   * The result should be set according to the operation 
   * specified in ieee->Cause and the result formatted as 
   * specified in ieee->Result.
   */

  /* ... */
}
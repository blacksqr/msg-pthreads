Msg {
  msgId = 1;
  hdr = Rc {
    nm =  "my_msg1_HdrS";
    id = 31415;
    bmHdr = BM14_FE43;
  };
  body = Rc {
      myt = AR8 [
		 3=>St {
		     iT03 = FV3 [ 21, 22, -21 ];
		     ii = 33;
		     bmTst02 = BM12_ABC3;
		 },
		 5=>St {
		     iT03 = FV3 [ 31, 32, -31 ];
		     ii = 44;
		     bmTst02 = BM12_7BC3;
		 }
		];
    iTst03 = AR8 [ 2=>21, 3=>31, 5=>41 ];
    nn = 31415;
    m = 21;
    sf06 =  "Msg1_BdStrSf06_1";
  };
};

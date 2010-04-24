Msg {
  msgId = 1;
  hdr = Rc {
    nm =  "my_msg1_HdrS";
    id = 31415;
    bmHdr = BM14_FE43;
  };
  body = Rc {
      myt = AR8 [
	 0=>St {
	     fT03 = V3 [ 2.11, 2.22, 2.33 ];
	     iT03 = FV3 [ 11, -111, -71 ];
	     ii = 11;
	     bmTst02 = BM12_ABC3;
	 }, 2=>St {
	     fT03 = V2 [ 3.11, 3.22 ];
	     iT03 = FV3 [ 21, 22, -21 ];
	     ii = 22;
	     bmTst02 = BM12_ABC3;
	 }, 3=>St {
	     fT03 = V3 [ 4.11, 4.22, 4.33 ];
	     iT03 = FV3 [ 21, 22, -21 ];
	     ii = 33;
	     bmTst02 = BM12_ABC3;
	 }, 5=>St {
	     fT03 = V3 [ 2.11, 2.22, 2.33 ];
	     iT03 = FV3 [ 35, 55, -51 ];
	     ii = 44;
	     bmTst02 = BM12_73F;
	 }, 6=>St {
	     fT03 = V4 [ 6.11, 6.22, 6.33, 6.66 ];
	     iT03 = FV3 [ -21, 222, -21 ];
	     ii = 77;
	     bmTst02 = BM12_AB4;
	 }
      ];
    iTst03 = AR8 [ 1=>11, 2=>21, 3=>31, 5=>41, 6=>66 ];
    nn = 31415;
    m = 21;
    un0 = Un {
      hh1 = Rc {
        nm = "my_str_union";
        id = 4321;
        bmHdr = BM14_0FF0;
      };
    };
    sf06 =  "Msg1_BdStrSf06_1";
  };
};

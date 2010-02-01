# $Id: model_file1.tcl 342 2010-01-25 17:28:36Z asus $
# Model def. file
#

_record hdr2 {
    CFieldUI4    nmid
    CFieldUI2    id
}

# comment
_record tst2 {
    TArray<hdr1,4>   atTst04
    TVector<CString> strTst05
    hdr1             vHdr1
    TFString<16>     sf06
}

# Comment
_struct tst1 {  # Comment test
    TVector<CFieldFI4>     fT03
    TFVector<CFieldSI4,3>  iT03
    CFieldSI2           ii
    TBitMap<12>         bmTst02
}

_record hdr1 {
    TFString<12> nm
    CFieldUI4    id
    TBitMap<14>  bmHdr
}

# Comment
_record tst3 {
    TArray<tst1,8>  myt
    TArray<CFieldSI4,8> iTst03
    CFieldUI4       nn
    CFieldUI2       m
    TFString<16>    sf06
}

_message mymsg1 {
    hdr1  hdr
    tst3  body
}

_message mymsg2 {
    hdr2 hdr
    tst1  body
}

_factory myFctr1 {
    mymsg1
    mymsg2
}

#
#LuaIf $DataType - gen lua interface
#
#

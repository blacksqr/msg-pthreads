#!/usr/bin/perl
#-w
# The contents of this file are subject to the Mozilla Public License
# Version 1.1 (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
# License for the specific language governing rights and limitations
# under the License.
# The Initial Developer of the Original Code is Alex Goldenstein.
# All Rights Reserved.
# Contributor(s): Alex Goldenstein.<goldale.de@googlemail.com>
#

my $modelDir = "./model";
my $genDir   = "./gen";

my $incDir   = "./inc";
my $srcDir   = "./src";

my $text     = "";

##############################################################

my $gStruct = q(
class CStructGen_$Nm {
  // disable copy constructor.
  CStructGen_$Nm(const CStructGen_$Nm&);
  void operator = (const CStructGen_$Nm&);

 protected:
  // Member list (sorted in mandatory order)
  // Big el. first & abc for el-ts of the same size
  <<<$TypeNm $ValNm;>>>
  TBitMap<$N> bm;  // BitMap
  bool operator() (uInt1 k) { return bm(k); }
 public:
  CStructGen_$Nm() {}
  CStructGen_$Nm(char* bf) { bf = reStore(bf); }
  ~CStructGen_$Nm() {}

  enum enumCStructGen_$Nm {
    <<<$ValNm_id ,>>>
    Last_Var_id
  };

  char* reStore(char* bf) {
    bf = bm.reStore(bf);
    TBitMapIt<$N> bIt(bm);
    <<<if(bIt()) bf = $ValNm.reStore(bf);  bIt.nxt();>>>
    return bf;
  }
  char* store(char* bf) {
    TBitMapIt<$N> bIt(bm);
    bf = bm.store(bf);
    <<<if(bIt()) bf = $ValNm.store(bf);  bIt.nxt();>>>
    return bf;
  }
  <<<$TypeNm* get_$ValNm()       { return (bm((uInt1)$ValNm_id)) ? (& $ValNm) : NULL; }>>>
  //<<<void set_$ValNm($TypeNm& v) { $ValNm.set(v); bm.set($ValNm_id); }>>>
  <<<void  set_$ValNm()          { bm.set($ValNm_id); }>>>
  <<<void uset_$ValNm()          { bm.uset($ValNm_id); }>>>

#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp __mi("^St \{ ",0x1);
    if(!__mi.exe(bf)) {
      CRegExp __me("^}([;,]? |;)",0x1);
      CRegExp __m("^([^ ]+) = ",0x2);
      bm.reset();
      bf = __mi.end(0x0);
      while(__me.exe(bf)) {
        if(!__m.exe(bf)) {
          *(__m.end(1))='\0';
          <<<if(!strcmp(__,$ValNm__,,__m(1))) { bf = $ValNm.fromStr(__m.end(0)); bm.set($ValNm_id); continue; }>>>
        } else return NULL;
      }
      return __me.end(0x0);
    }
    return NULL;
  }
  char* dump(char* bf,uInt1 of) {
    TBitMapIt<$N> bIt(bm);
    strcpy(bf, "St {\n"); bf += 5; of += 2;
    <<<bf+=::sprintf(bf,"%s$ValNm = ",_prnDmpOffSet);bf=bIt()?$ValNm.dump(bf,of):bf;strcpy(bf,";\n");bf+=2;>>>
    of -= 2; strcpy(bf, _prnDmpOffSet); bf += of;
    strcpy(bf++, "}");
    return bf;
  }
  char* schema(char* bf,uInt1 of) {
    strcpy(bf, "St {\n"); bf += 5; of += 2;
    <<<bf+=::sprintf(bf,"%s$ValNm = ",_prnDmpOffSet);bf=$ValNm.schema(bf,of);strcpy(bf,";\n");bf+=2;>>>
    of -= 2; strcpy(bf, _prnDmpOffSet); bf += of;
    strcpy(bf++, "}");
    return bf;
  }
#endif // USE_DUMP
};);

##############################################################

my $gRecrd = q(
class CRecrdGen_$Nm {
  // disable copy constructor.
  CRecrdGen_$Nm(const CRecrdGen_$Nm&);
  void operator = (const CRecrdGen_$Nm&);

 protected:
  // Member list (sorted in mandatory order)
  <<<$TypeNm $ValNm;>>>
 public:
  CRecrdGen_$Nm() {}
  CRecrdGen_$Nm(char* bf) { bf = reStore(bf); }
  ~CRecrdGen_$Nm() {}

  char* reStore(char* bf) {
    // Statment's list
    <<<bf = $ValNm.reStore(bf);>>>
    return bf;
  }
  char* store(char* bf) {
    <<<bf = $ValNm.store(bf);>>>
    return bf;
  }
  <<<$TypeNm* get_$ValNm()       { return & $ValNm; }>>>
  //<<<void set_$ValNm($TypeNm& v) { $ValNm.set(v); }>>>

#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp __mi("^Rc \{ ",0x1);
    if(!__mi.exe(bf)) {
      CRegExp __me("^}([;,]? |;)",0x1);
      CRegExp __m("^([^ ]+) = ",0x2);
      bf = __mi.end(0x0);
      while(__me.exe(bf)) {
        if(!__m.exe(bf)) {
          *(__m.end(1))='\0';
          <<<if(!strcmp(__,$ValNm__,,__m(1))) { bf = $ValNm.fromStr(__m.end(0)); continue; }>>>
        } else return NULL;
      }
      return __me.end(0x0);
    }
    return NULL;
  }
  char* dump(char* bf,uInt1 of) {
    strcpy(bf, "Rc {\n"); bf+=5; of+=2;
    <<<bf+=::sprintf(bf,"%s$ValNm = ",_prnDmpOffSet);bf=$ValNm.dump(bf,of);strcpy(bf,";\n");bf+=2;>>>
    of -= 2; strcpy(bf,_prnDmpOffSet); bf += of;
    strcpy(bf++, "}");
    return bf;
  }
  char* schema(char* bf,uInt1 of) {
    strcpy(bf, "Rc {\n"); bf+=5; of+=2;
    <<<bf+=::sprintf(bf,"%s$ValNm = ",_prnDmpOffSet);bf=$ValNm.schema(bf,of);strcpy(bf,";\n");bf+=2;>>>
    of -= 2; strcpy(bf,_prnDmpOffSet); bf += of;
    strcpy(bf++, "}");
    return bf;
  }
#endif // USE_DUMP
};);

##############################################################

my $gMsg = q(
class CMsgGen_$Nm : public CMessage {
  // disable copy constructor.
  CMsgGen_$Nm(const CMsgGen_$Nm&);
  void operator = (const CMsgGen_$Nm&);

  friend class CMsgFactory;

 protected:
  $BodyTypeNm body;
  $HdrTypeNm hdr;

 public:
  CMsgGen_$Nm(char* p=NULL) : CMessage(p, CMsgGen_$Nm_id) { /* bHndl = 0u; */ }
  virtual ~CMsgGen_$Nm() {}

  // Get ref. to member
  $HdrTypeNm&  getHdr()  { return hdr; }
  $BodyTypeNm& getBody() { return body; }

  virtual char parse0() { // PreParse to allow message dispatching
    mBuff = hdr.reStore(mBuff);
    return mBuff ? '\0' : 'x';
  }
  virtual char parse1() { // Parse rest of the message
    char rr = '\0';
    mBuff = body.reStore(mBuff);
    rr = mBuff ? '\0' : 'x';
    // signal to release mBuff pointer
    relsBuff();
    return rr;
  }
  virtual char* store(char* bf) {
    char* tb = bf;
    const sInt2 hdrSize = 2*sizeof(uInt2);
    bf = hdr.store(bf + hdrSize);
    bf = body.store(bf);
    CFieldUI2 mLen((bf - tb) - hdrSize);
    tb = mLen.store(tb);
    msgId.store(tb);
    return bf;
  }

#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp __mi("^Msg \{ msgId = ([0-9]+); hdr = ",0x2);
    if(!__mi.exe(bf)) {
      CRegExp __me("^}; ?",0x1);
      CRegExp __m("^([^ ]+) = ",0x2);
      *(__mi.end(0x1)) = '\0'; 
      uInt2 bid = atoi(__mi(0x1)); 
      if(bid != msgId()) { 
        printf("Error - wrong message type msgId=%u\n",bid); 
        return NULL; 
      } 
      bf = __mi.end(0x1) + 2;
      if(!__m.exe(bf)) {
        bf=hdr.fromStr(__m.end(0));
      } else return NULL;
      if(!__m.exe(bf)) {
        bf=body.fromStr(__m.end(0));
      } else return NULL;
      if(!__me.exe(bf))
        return __me.end(0x0);
    }
    return NULL;
  }
  char* dump(char* bf) {
    uInt1 of = 2u;
    strcpy(bf, "Msg {\n"); bf+=6;
    bf+=::sprintf(bf,"%smsgId = %u;\n",_prnDmpOffSet,msgId());
    bf+=::sprintf(bf,"%shdr = ",_prnDmpOffSet);
    bf=hdr.dump(bf,of); strcpy(bf,";\n");bf+=2;
    bf+=::sprintf(bf,"%sbody = ",_prnDmpOffSet);
    bf=body.dump(bf,of);strcpy(bf,";\n");bf+=2;
    of-=2; strcpy(bf,_prnDmpOffSet); bf+=of;
    strcpy(bf, "};\n");
    return (bf+3);
  }
  char* schema(char* bf) {
    uInt1 of = 2u;
    strcpy(bf, "Msg {\n"); bf+=6;
    bf+=::sprintf(bf,"%smsgId = %u;\n",_prnDmpOffSet,msgId());
    bf+=::sprintf(bf,"%shdr = ",_prnDmpOffSet);
    bf=hdr.schema(bf,of); strcpy(bf,";\n");bf+=2;
    bf+=::sprintf(bf,"%sbody = ",_prnDmpOffSet);
    bf=body.schema(bf,of);strcpy(bf,";\n");bf+=2;
    of-=2; strcpy(bf,_prnDmpOffSet); bf+=of;
    strcpy(bf, "};\n");
    return (bf+3);
  }
#endif // USE_DUMP
};);

##############################################################

my $gFctry = q(
class CMessage;

class CFctryGen_$FN : public CMsgFactory {
  // disable copy constructor.
  CFctryGen_$FN(const CFctryGen_$FN&);
  void operator = (const CFctryGen_$FN&);

 public:
  CFctryGen_$FN() {}
  virtual ~CFctryGen_$FN() {}

  // return - bf. pointer to the next message
  virtual CMessage* reStore(char*& bf) {
    CFieldUI2 mLen;
    CFieldUI2 msgId;
    bf = mLen.reStore(bf);
    bf = msgId.reStore(bf);
    char* p = bf;
    bf += mLen.get();
    //printf("CMsgFactory::reStore  Len=%u  mId=%u\n",mLen(),msgId());
    switch(msgId.get()) {
      <<<case CMsgGen_$Nm_id: { return new CMsgGen_$Nm(p); }>>>
    }
    return NULL; // Error
  }
};);

##############################################################

my %type2inc = {};
my %IncFileHash = {};

sub notBaseType {
    my ($tp) = @_;
    #print ">>> notBaseType>>> $tp\n";
    if($tp =~ m/(CField[US]I[1248]|CFieldF[48]|CString|TFString<\d+>|TBitMap<\d+>|TArray<([^,]+),\d+>|TFVector<([^,]+),\d+>|TVector<([^>]+)>)/)
    {
	#print "   >>> notBaseType>>> $tp >>$1 >>$2\n";
	if(defined($2)) {
	    return notBaseType($2);
	}
    } else {
	return $tp;
    }
}

my %ShMsg;
my %ShFctr;
my %ShStruct;
my %ShRec;

# Get list of model files - *.tcl

@mdList = <./model/*.tcl>;

foreach $mf ( @mdList ) {
    print "\nProcess FILE - $mf\n";

    open(MF_, "< $mf") or die "Couldn't open $mf for reading: $!\n";

    $pMode = 0;
    while (<MF_>) {
	chomp;              # no newline
	s/#.*//;            # no comments
	s/^\s+//;           # no leading white
	s/\s+$//;           # no trailing white
	next unless length; # anything left?
	s/\s+/ /g;          # reduce to one space

	#print "LINE =>$_<=\n";
	if( ! $pMode ) {
	    # struct header
	    if(m/^_([^ ]+) ([^ ]+) ?{$/) {
		$pMode = 1;
		$Strct = $1;
		$StrctNm = $2;
		print "  Process - $Strct $StrctNm\n";
		if($Strct eq "message") {
		    $ShMsg{$StrctNm} = ();
		} elsif($Strct eq "factory") {
		    $ShFctr{$StrctNm} = ();
		} elsif($Strct eq "record") {
		    $ShRec{$StrctNm} = ();
		} elsif($Strct eq "struct") {
		    $ShStruct{$StrctNm} = ();
		} else {
		    print "*** ERROR - <$Strct> unknown <$pMode>\n";
		}
	    }
	} else {
	    if($_ eq "}") {
		$pMode = 0;
		#print "\$type2inc{ $StrctNm } = $Strct\_$StrctNm;\n";
		$type2inc{ $StrctNm } = "$Strct\_$StrctNm";
		next;
	    }
	    # struct body
	    my ($vType, $vnm) = split(" ", $_, 2);
	    $xx = notBaseType($vType);
	    if( $xx ne "" ) {
		#print "notBaseType\($vType\) ==>> $xx\n";

		# Include file list
		my $vv = "$Strct\_$StrctNm";
		#print "push\(\@{\$IncFileHash{ $vv }}, $xx\);\n";
		push(@{$IncFileHash{$vv}}, $xx);
	    }
	    if($Strct eq "message") {
		push(@{$ShMsg{$StrctNm}}, $_);
		#print "message - push\(\@{\$ShMsg{ $StrctNm }}, $_ \)\n";
	    } elsif($Strct eq "factory") {
		push(@{$ShFctr{$StrctNm}}, $_);
	    } elsif($Strct eq "record") {
		push(@{$ShRec{$StrctNm}}, $_);
	    } elsif($Strct eq "struct") {
		push(@{$ShStruct{$StrctNm}}, $_);
	    } else {
		print "*** ERROR - >$Strct< unknown <$pMode>\n";
	    }
	    push(@sBody, $_);
	}
    }
}

##############################################################

# Process Struct
$N = 0;
$StructType = "struct";
my ($x, $TypeNm, $ValNm) = ("", "", "");

sub tmplResv {
    my ($a, $x) = @_;
    my $z = $x;
    $x =~ s/(\$[^ ,_;&<>{}\.\*\(\)\[\]]+)/$1/gee;
    #print "=tmplResv= <$a> <$z> <$x> <$TypeNm> <$ValNm> \"$a$x\n$a<<<$z>>>\"\n";
    return "$a$x\n$a<<<$z>>>";
}

while(($Nm, $x) = each( %ShStruct )) {
    @sBody = @$x;
    $N     = $#sBody;
    $text  = $gStruct;

    #$xx = join(" >", @sBody);
    #print ">> Struct> $Nm >> $xx\n";

    foreach $ln ( @sBody ) {
	($TypeNm, $ValNm) = split(" ", $ln, 2);
	print ">>ln> $TypeNm <> $ValNm\n";

	# Replace $TypeNm with full TypeName
	$xx = notBaseType($TypeNm);
	$x = $xx;
	if( $xx ne "" ) {

	    $xx = $type2inc{$xx};
	    $xx =~ s/struct_/CStructGen_/g;
	    $xx =~ s/record_/CRecrdGen_/g;
	    $xx =~ s/message_/CMsgGen_/g;
	    $xx =~ s/factory_/CFctryGen_/g;

	    $TypeNm =~ s/$x/$xx/ge;
	    print ">>ln> $TypeNm <> $ValNm\n";
	}

	$text =~ s{([^\n]*)<<<([^>]+)>>>}{tmplResv($1, $2)}gex;
    }
    $text =~ s/[^\n]*<<<[^>]*>>>\n//g;
    $text =~ s/__,/"/g;

    $text =~ s/\$([^ ,_;&<>{}\.\*\(\)\[\]]+)/${$1}/g;

    # Include list
    $incList = "";
    $xx = "$StructType\_$Nm";
    foreach $ln ( @{$IncFileHash{$xx}} ) {
	#print "\nforeach $ln \( \@{\$IncFileHash{ $xx }} \) {\n";
	if(exists( $type2inc{$ln} )) {
	    #print "*** <$type2inc{$ln}.h>\n";
	    $incList = "$incList\n#include <$type2inc{$ln}.h>";
	} else {
	    print "*** ERROR - undef type >> $ln\n";
	}
    }
    #print "***\n$incList\n";

    my $mPref = "#ifndef MSG_PARSER_$StructType\_$Nm\_H
#define MSG_PARSER_$StructType\_$Nm\_H\n\n#include <msgParser.h>\n$incList\n
namespace MsgModel {\n";
    my $mSuff = "\n}  // namespace MsgModel\n
#endif // MSG_PARSER_$StructType\_$Nm\_H\n\n";

    open(HFF, "> ./gen/$StructType\_$Nm.h")
    or die "Couldn't open $StructType\_$Nm.h for writing: $!\n";

    print HFF "$mPref\n// Gen-Struct - $Nm $N\n$text\n$mSuff";
    close HFF;
}

##############################################################

# Process Rec
$StructType = "record";
($N, $x, $TypeNm, $ValNm) = (0, "", "", "");

while(($Nm, $x) = each( %ShRec )) {
    @sBody = @$x;
    $N     = $#sBody;
    $text  = $gRecrd;

    #$xx = join(" >", @sBody);
    #print ">> Record> $Nm-$N >> $xx\n";

    foreach $ln ( @sBody ) {
	($TypeNm, $ValNm) = split(" ", $ln, 2);
	#print ">>ln> $ln>>    $TypeNm <> $ValNm\n";

	# Replace $TypeNm with full TypeName
	$xx = notBaseType($TypeNm);
	$x = $xx;
	if( $xx ne "" ) {

	    $xx = $type2inc{$xx};
	    $xx =~ s/struct_/CStructGen_/g;
	    $xx =~ s/record_/CRecrdGen_/g;
	    $xx =~ s/message_/CMsgGen_/g;
	    $xx =~ s/factory_/CFctryGen_/g;

	    $TypeNm =~ s/$x/$xx/ge;
	    print ">>ln> $TypeNm <> $ValNm\n";
	}

	$text =~ s{([^\n]*)<<<([^>]+)>>>}{tmplResv($1, $2)}gex;
    }
    $text =~ s/[^\n]*<<<[^>]*>>>\n//g;
    $text =~ s/__,/"/g;

    $text =~ s/\$([^ ,_;&<>{}\.\*\(\)\[\]]+)/${$1}/g;

    # Include list
    $incList = "";
    $xx = "$StructType\_$Nm";
    foreach $ln ( @{$IncFileHash{$xx}} ) {
	#print "\nforeach $ln \( \@{\$IncFileHash{ $xx }} \) {\n";
	if(exists( $type2inc{$ln} )) {
	    #print "*** <$type2inc{$ln}.h>\n";
	    $incList = "$incList\n#include <$type2inc{$ln}.h>";
	} else {
	    print "*** ERROR - undef type >> $ln\n";
	}
    }
    #print "***\n$incList\n";

    my $mPref = "#ifndef MSG_PARSER_$StructType\_$Nm\_H
#define MSG_PARSER_$StructType\_$Nm\_H\n\n#include <msgParser.h>\n$incList\n
namespace MsgModel {\n";
    my $mSuff = "\n}  // namespace MsgModel\n
#endif // MSG_PARSER_$StructType\_$Nm\_H\n\n";

    open(HFF, "> ./gen/$StructType\_$Nm.h")
    or die "Couldn't open $StructType\_$Nm.h for writing: $!\n";

    print HFF "$mPref\n// Gen-Struct - $Nm $N\n$text\n$mSuff";
    close HFF;
}

##############################################################

# Process Msg
$StructType = "message";
($N, $x, $TypeNm, $ValNm) = (0, "", "", "");

@MsgGenId_enum = ();

while(($Nm, $x) = each( %ShMsg )) {
    @sBody = @$x;
    $N     = $#sBody;
    $text  = $gMsg;

    #$xx = join(" >", @sBody);
    #print ">> Message> $Nm >> $xx\n";

    $xx = "CMsgGen\_$Nm\_id";
    push(@MsgGenId_enum, $xx);

    $HdrTypeNm = $BodyTypeNm = "";
    foreach $ln ( @sBody ) {
	($TypeNm, $ValNm) = split(" ", $ln, 2);

	# Replace $TypeNm with full TypeName
	$xx = notBaseType($TypeNm);
	$x = $xx;
	if( $xx ne "" ) {

	    $xx = $type2inc{$xx};
	    $xx =~ s/struct_/CStructGen_/g;
	    $xx =~ s/record_/CRecrdGen_/g;
	    $xx =~ s/message_/CMsgGen_/g;
	    $xx =~ s/factory_/CFctryGen_/g;

	    $TypeNm =~ s/$x/$xx/ge;
	    print ">>ln> $TypeNm <> $ValNm\n";
	}

	if($ValNm eq "hdr") {
	    $HdrTypeNm = $TypeNm;
	} elsif($ValNm eq "body") {
	    $BodyTypeNm = $TypeNm;
	}
    }
    #print ">> Message> Hdr-$HdrTypeNm  Body-$BodyTypeNm\n";
    $text =~ s/\$([^ ,_;&<>{}\.\*\(\)\[\]]+)/${$1}/g;

    # Include list
    $incList = "";
    $xx = "$StructType\_$Nm";
    foreach $ln ( @{$IncFileHash{$xx}} ) {
	#print "\nforeach $ln \( \@{\$IncFileHash{ $xx }} \) {\n";
	if(exists( $type2inc{$ln} )) {
	    #print "*** <$type2inc{$ln}.h>\n";
	    $incList = "$incList\n#include <$type2inc{$ln}.h>";
	} else {
	    print "*** ERROR - undef type >> $ln\n";
	}
    }
    #print "***\n$incList\n";

    my $mPref = "#ifndef MSG_PARSER_$StructType\_$Nm\_H
#define MSG_PARSER_$StructType\_$Nm\_H\n\n#include <msgParser.h>\n$incList\n
namespace MsgModel {\n";
    my $mSuff = "\n}  // namespace MsgModel\n
#endif // MSG_PARSER_$StructType\_$Nm\_H\n\n";

    open(HFF, "> ./gen/$StructType\_$Nm.h")
    or die "Couldn't open $StructType\_$Nm.h for writing: $!\n";

    print HFF "$mPref\n// Gen-Struct - $Nm $N\n$text\n$mSuff";
    close HFF;
}

##############################################################

# Process Factry
$StructType = "factory";
($N, $x, $TypeNm, $ValNm) = (0, "", "", "");

while(($FN, $x) = each( %ShFctr )) {
    @sBody = @$x;
    $N     = $#sBody;
    $text  = $gFctry;

    #$xx = join(" >", @sBody);
    #print ">> Factory> $FN >> $xx\n";

    foreach $Nm ( @sBody ) {
	#print ">> $Nm\n";
	$text =~ s{([^\n]*)<<<([^>]+)>>>}{tmplResv($1, $2)}gex;
    }
    $text =~ s/[^\n]*<<<[^>]*>>>\n//g;

    $text =~ s/\$([^ ,_;&<>{}\.\*\(\)\[\]]+)/${$1}/g;

    # Include list
    $incList = "";
    $xx = "$StructType\_$FN";
    foreach $ln ( @{$IncFileHash{$xx}} ) {
	#print "\nforeach $ln \( \@{\$IncFileHash{ $xx }} \) {\n";
	if(exists( $type2inc{$ln} )) {
	    #print "*** <$type2inc{$ln}.h>\n";
	    $incList = "$incList\n#include <$type2inc{$ln}.h>";
	} else {
	    print "*** ERROR - undef type >> $ln\n";
	}
    }
    #print "***\n$incList\n";

    my $mPref = "#ifndef MSG_PARSER_$StructType\_$FN\_H
#define MSG_PARSER_$StructType_$FN\_H\n\n#include <msgParser.h>\n$incList\n
namespace MsgModel {\n";
    my $mSuff = "\n}  // namespace MsgModel\n
#endif // MSG_PARSER_$StructType\_$FN\_H\n\n";

    open(HFF, "> ./gen/$StructType\_$FN.h")
    or die "Couldn't open $StructType\_$FN.h for writing: $!\n";

    print HFF "$mPref\n// Gen-Struct - $FN $N\n$text\n$mSuff";
    close HFF;
}

##############################################################

# Global model const

$text = join(",\n  ", @MsgGenId_enum);
$text = "enum ModelGenMsgId {\n  CMsgGen_DummyId = 0,\n  $text\n};";

my $mPref = "#ifndef MSG_PARSER_MdlGlobConst_H
#define MSG_PARSER_MdlGlobConst_H\n\nnamespace MsgModel {\n";
my $mSuff = "\n}  // namespace MsgModel\n
#endif // MSG_PARSER_MdlGlobConst_H\n\n";

open(HFF, "> ./gen/ModelConst.h")
    or die "Couldn't open $StructType\_$FN.h for writing: $!\n";

print HFF "$mPref\n// Gen-Struct - MdlGlobConst\n$text\n$mSuff";
close HFF;

#####################
### ToLua binding ###
#####################
#

# $Id: modelGen.pl 350 2010-02-08 18:38:31Z asus $

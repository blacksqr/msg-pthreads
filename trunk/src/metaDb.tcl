#!./tclSql

file delete /tmp/ITSs.db
file delete /tmp/ITSs.db-journal
file delete /tmp/its.log

sqlite3 db /tmp/ITSs.db

db eval {
    PRAGMA default_synchronous=OFF;
    PRAGMA default_temp_store=MEMORY;
    PRAGMA default_cache_size=16384;
    PRAGMA legacy_file_format = OFF;
    BEGIN; -- begin transaction
}

if 0 {
    db eval {
	CREATE TABLE _TelKard
	(
	 id       INTEGER PRIMARY KEY,
	 pin      INTEGER UNIQUE,
	 konto    INTEGER,
	 kredit   INTEGER, -- can be negative
	 type     INTEGER, -- 0xFFFF => 0x<ValutaId><TarifId><..><..>
	 proz     INTEGER, -- Our % of SailPrice
	 dPreis   INTEGER,
	 nCalls   INTEGER,
	 stat     INTEGER, -- Enabl/Disabl,Shared,...
	 tarif    INTEGER, -- TaryfID in TCL script (wrkThread)
	 tmLimit  INTEGER, -- MAX day - card can be used after 1-call, 0 => no limit
	 timeOpen INTEGER,
	 time1Cl  INTEGER);
    }
    puts ">>>> TelKard"

    db eval {
	-- First record => SELF
	CREATE TABLE _Provider
	(
	 id       INTEGER PRIMARY KEY,  -- 1 - Our KONTO
	 name     TEXT UNIQUE, -- Out port-group name for routing
	 -- SELF - our Konto for trafic routing
	 konto    INTEGER,     -- in 0.001 Cent
	 kredit   INTEGER,     -- in 1 EUR
	 ser      INTEGER,     -- Traffik Gewin (100% = 10000) of SailPrice
	 type     INTEGER,     -- BitFlag - 0x01 => On/Off; 0x02 => VoIP/PSTN
	 tmOpen   INTEGER);
    }
    puts ">>>> Provider"

    db eval {
	CREATE TABLE _CntryCode
	(
	 id     INTEGER UNIQUE,
	 parent INTEGER,
	 myId   INTEGER,   -- id for Sail-Prices
	 code   INTEGER,   -- Sample => 49690000000 - Frankfurt
	 len    INTEGER,
	 name   TEXT);     -- City/Cuntry name
	CREATE INDEX CnCode ON _CntryCode(code);
    }
    puts ">>>> CntryCode"

    db eval {
	CREATE TABLE _TmSgmPrv
	(
	 id       INTEGER PRIMARY KEY,
	 cId      INTEGER,  -- CntryCode id
	 prId     INTEGER,  -- Provider id, SelfId = 1 / my trafik tarif
	 wkTime   INTEGER,  -- Min. of Week
	 preis    INTEGER,  -- min's price
	 startT   INTEGER,
	 endT     INTEGER,
	 prio     INTEGER); -- prio=0(default), 1,2,3..., 65,66,... - myTrafPrices
	CREATE INDEX SgmCnIdPr ON _TmSgmPrv(cId);
	-- TRIGGER
	CREATE TRIGGER TmSgm_t AFTER INSERT ON _TmSgmPrv
	BEGIN
	UPDATE _TmSgmPrv SET endT=new.startT WHERE cId=new.cId AND prId=new.prId;
	END;
    }
    puts ">>>> TmSgmPrv"

    db eval {
	-- (cId + tarif + wkTime) => Unique Key
	CREATE TABLE _TmSgmMy
	(
	 id       INTEGER PRIMARY KEY,
	 cId      INTEGER,
	 tarif    INTEGER,
	 -- wkTime from 0 to 60*24*7=10080
	 -- wkTime - is a start Point of segment
	 wkTime   INTEGER,  -- Minute of Week
	 conPrs   INTEGER,  -- Conn. price
	 conPrsM  INTEGER,  -- Conn. price Mob.
	 preis    INTEGER,  -- min preis
	 preisM   INTEGER,
	 startT   INTEGER,
	 endT     INTEGER);
	CREATE INDEX SgmCnIdMy ON _TmSgmMy(cId);
    }
    puts ">>>> TmSgmMy"

    db eval {
	CREATE TABLE _InCall
	(
	 id       INTEGER PRIMARY KEY,
	 cli      INTEGER,
	 dli      INTEGER,
	 tcId     INTEGER,  -- TelCard ID
	 preis    INTEGER,
	 start    INTEGER,
	 duration INTEGER,  -- in Sec
	 port     INTEGER,  -- incomming port
	 cause    INTEGER); -- clearing cause
	CREATE INDEX InClCli  ON _InCall(cli);
	CREATE INDEX InClStrt ON _InCall(start);
	CREATE INDEX InClKId  ON _InCall(tcId);
    }
    puts ">>>> InCall"

    db eval {
	CREATE TABLE _OutCall
	(
	 inClId   INTEGER,
	 phon     INTEGER,  -- DLI
	 kPreis   INTEGER,
	 vPreis   INTEGER,
	 kPrSegm  INTEGER,
	 vPrSegm  INTEGER,
	 start    INTEGER,
	 duration INTEGER,  -- in Sec
	 port     INTEGER,  -- outgoing port
	 cause    INTEGER); -- clearing cause
	CREATE INDEX OutClId   ON _OutCall(inClId);
	CREATE INDEX OutClDli  ON _OutCall(phon);
	CREATE INDEX OutClStrt ON _OutCall(start);
    }
    puts ">>>> OutCall"

    db eval {
	CREATE TABLE _TrafCall
	(
	 cli      INTEGER,
	 dli      INTEGER,
	 kIPreis  INTEGER,
	 vIPreis  INTEGER,
	 kOPreis  INTEGER,
	 vOPreis  INTEGER,
	 kPrSegm  INTEGER,
	 vPrSegm  INTEGER,
	 kPId     INTEGER,  -- Kauf Provider
	 start    INTEGER,
	 vPId     INTEGER,  -- Verk Provider
	 inPId    INTEGER,  -- In Provider
	 duration INTEGER,  -- in Sec
	 inPort   INTEGER,
	 outPort  INTEGER,
	 inCause  INTEGER,  -- clearing cause
	 outCause INTEGER);
	CREATE INDEX TrfClStrt ON _TrafCall(start);
    }
    #CREATE INDEX TrfClDli  ON _TrafCall(dli);
    puts ">>>> TrafCall"

    db eval {
	CREATE TABLE _KardLot
	(
	 id       INTEGER PRIMARY KEY,
	 name     TEXT,       -- lot name
	 cardId   INTEGER,
	 konto    INTEGER,
	 start    INTEGER,
	 changed  INTEGER,
	 kredit   INTEGER,    -- can be negative
	 pId      INTEGER,    -- Provider ID (card's owner)
	 type     INTEGER,    -- 0xFFFF => 0x<ValutaId><TarifId><..><..>
	 proz     INTEGER,    -- Our % of SailPrice
	 menge    INTEGER,
	 nActiv   INTEGER,
	 tarif    INTEGER,    -- TaryfID in TCL script (wrkThread)
	 tmLimit  INTEGER,    -- MAX day - card can be used after 1 call
	 dPreis   INTEGER,    -- Tagesgebuer
	 memo     TEXT);
	CREATE INDEX kLotNm ON _KardLot(name);
    }
    puts ">>>> KardLot"

    db eval {
	CREATE TABLE _LotStory
	(
	 lId     INTEGER,  -- Lot ID
	 updat   INTEGER,
	 idFrom  INTEGER,
	 idBis   INTEGER,
	 konto   INTEGER,
	 kredit  INTEGER,
	 proz    INTEGER,
	 memo    TEXT);
	CREATE INDEX LtStrLt ON _LotStory(lId);
	CREATE INDEX LtStrFr ON _LotStory(idFrom);
    }
    puts ">>>> LotStory"

    # Uberweisung history
    db eval {
	CREATE TABLE _GeldMove
	(
	 PrvdrId INTEGER,  -- 0 - eigene Einzahlung
	 summa   INTEGER,
	 time    INTEGER,
	 memo    TEXT);
	CREATE INDEX GldTrPId ON _GeldMove(PrvdrId);
    }
    puts ">>>> GldTransf"

    # Kontostate history
    db eval {
	CREATE TABLE _KntoHist
	(
	 time    INTEGER,
	 konto   INTEGER,
	 pId     INTEGER); -- Prvdr ID
	CREATE INDEX KHstTm ON _KntoHist(time);
    }
    puts ">>>> KntoHist"

    # CLI history
    db eval {
	--  *** flags for preSel field ***
	--  p_Preselect = 0x01  => PIN is stored
	--  p_Shared    = 0x02  => Multiuser PIN
	--  p_Locked    = 0x04  => PIN blocked
	--  p_VirtCLI   = 0x08  => not locable
	--  p_PremCall  = 0x10  => try to make Premium call 
	--  p_CByCall   = 0x20  => for trafik CallByCall => nBadCalls = TarifID
	CREATE TABLE _CLI(
			  cli      INTEGER PRIMARY KEY,
			  TKard    INTEGER,
			  nBCall   INTEGER,
			  preSel   INTEGER,
			  updat    INTEGER);
	CREATE INDEX CliTKrd ON _CLI(TKard);
    }
    puts ">>>> CLI"

    db eval {
	-- calls statistic
	CREATE TABLE _ClStat
	(
	 cId      INTEGER,
	 prvId    INTEGER,
	 nCall    INTEGER,
	 nOk      INTEGER,  -- call time > 53 s
	 ncl20    INTEGER,  -- call time > 20 s
	 nBad     INTEGER,  -- outCall not connected
	 totTime  INTEGER,  -- total time in sec
	 start    INTEGER); -- Statistic started
	CREATE INDEX ClStatCId ON _ClStat(cId);
    }
    puts ">>>> ClStat"

    # support for prise independet routing
    db eval {
	CREATE TABLE _Routing
	(
	 rId   INTEGER PRIMARY KEY,
	 cId   INTEGER,  -- CountryCode ID
	 beg   INTEGER,  -- Start time
	 end   INTEGER,  -- End Time
	 prvAr BLOB);
	CREATE INDEX RtngCId ON _Routing(cId);
    }
    puts ">>>> Routing"
}

# Remote user autothication
db eval {
    CREATE TABLE _RemUser
    (
     uid   TEXT PRIMARY KEY,
     pwd   TEXT,    -- User password
     xTxt  TEXT,    -- User password
     pId   INTEGER, -- Provider ID
     IId   INTEGER, -- Provider ID
     memo  TEXT);
    CREATE INDEX indIid ON _RemUser(Iid);

    COMMIT; -- end transaction
}
puts ">>>> RemUser"
db close

# Rand-Numm-Gen => number 0-10000
set RND [open /dev/urandom r]
proc getRnd {} {
    global RND
    binary scan [read $RND 4] i* rNmr
    return [expr (($rNmr+0) & 0xFFFFFFF) / 0x68DB]
}
#set CalcTickTimes(0) { 20 40 60 120 180 }
#set ChrgTickTimes(0) { 1919 3923 5507 10717 16137 }
#set ChrgTickTimes(1) { 1987 3979 5933 5907 5701 }

if 0 {
    sqlite3 db /tmp/ITSs.db

    # SQL TEST ##############################################
    db eval {
        BEGIN;
        INSERT INTO _CLI VALUES(149694990673,777,0,0,1);
        INSERT INTO _CLI VALUES(1496994410668,333,0,0,4);
        COMMIT;
    }
    set x [db eval "SELECT * FROM _CLI;"]
    puts " 1 ** $x"
    db eval {
        BEGIN;
        UPDATE _CLI SET nBCall=nBCall+1 WHERE cli=149694990673;
        UPDATE _CLI SET nBCall=nBCall+1 WHERE cli=1496994410668;
        COMMIT;
    }
    set x [db eval "SELECT * FROM _CLI;"]
    puts " 2 ** $x"
    for {set kk 0} {$kk < 33} {incr kk} {
	set vv1 [getRnd]
	set vv2 [getRnd]
	db eval {
	    BEGIN;
	    INSERT INTO _CLI VALUES($vv1,777,0,0,1);
	    INSERT INTO _CLI VALUES($vv2,333,0,0,4);
	    COMMIT;
	}
	set x [db eval "SELECT * FROM _CLI;"]
	puts " 1 ** $x"
	db eval {
	    BEGIN;
	    UPDATE _CLI SET nBCall=nBCall+1 WHERE cli=$vv1;
	    UPDATE _CLI SET nBCall=nBCall+1 WHERE cli=$vv2;
	    COMMIT;
	}
	set x [db eval "SELECT * FROM _CLI;"]
    }
    #INSERT INTO _GeldMove VALUES(?,?,?,?);
    #UPDATE _CLI SET nBCall=nBCall+1 WHERE cli=?
    #INSERT INTO _KntoHist VALUES(?,?,?,?);

    #INSERT INTO _CLI VALUES(?,?,?,?,?);
    #UPDATE _CLI SET TKard=?,nBadCl=?,preSel=?,updat=? WHERE cli=?;

    #SELECT * FROM _ClStat WHERE cId=? AND prvId=?;
    #INSERT INTO _ClStat VALUES(?,?,?,?,?,?,?,?,?);
    #UPDATE _ClStat SET nCall=?,nOk=?,ncl20=?,nBad=?,totTime=? WHERE cId=? AND prvId=?;
    db close
}

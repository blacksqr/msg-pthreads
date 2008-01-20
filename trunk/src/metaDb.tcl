#!./tclSql

file delete /tmp/its_demo.db
file delete /tmp/its_demo.db-journal
file delete /tmp/its.log

sqlite3 db /tmp/its_demo.db

db eval {
    PRAGMA default_synchronous=OFF;
    PRAGMA default_temp_store=MEMORY;
    PRAGMA default_cache_size=16384;
    PRAGMA legacy_file_format = OFF;
    BEGIN; -- begin transaction

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

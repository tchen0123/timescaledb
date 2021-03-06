\set ON_ERROR_STOP 1
\set VERBOSITY verbose
\set SHOW_CONTEXT never


\set ECHO ALL
\ir include/insert_single.sql

\set VERBOSITY default
DO $$
BEGIN
    CREATE ROLE alt_usr LOGIN;
EXCEPTION
    WHEN duplicate_object THEN
        --mute error
END$$;

\c single alt_usr
\dt

\set ON_ERROR_STOP 0
SELECT * FROM chunk_closing_test;
--todo fix error message here:
SELECT * FROM "testNs";
\set ON_ERROR_STOP 1

CREATE TABLE "1dim"(time timestamp, temp float);
SELECT create_hypertable('"1dim"', 'time');
INSERT INTO "1dim" VALUES('2017-01-20T09:00:01', 22.5);
INSERT INTO "1dim" VALUES('2017-01-20T09:00:21', 21.2);
INSERT INTO "1dim" VALUES('2017-01-20T09:00:47', 25.1);
SELECT * FROM "1dim";


\ir include/ddl_ops_1.sql
\ir include/ddl_ops_2.sql

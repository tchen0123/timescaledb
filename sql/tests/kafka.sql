\set ON_ERROR_STOP 1

\ir create_clustered_db.sql
\c Test1
\ir load_kafka.sql
\c test2
\ir load_kafka.sql

\set ECHO ALL
\c meta
SELECT add_cluster_user('postgres', NULL);

SELECT add_node('Test1' :: NAME, 'localhost');
SELECT add_node('test2' :: NAME, 'localhost');


\c Test1
\dt public.*
\det+ public.*


SELECT * FROM kafka_get_start_and_next_offset('topic', 0::SMALLINT, 0);
SELECT * FROM kafka_get_start_and_next_offset('topic', 0::SMALLINT, 0);
SELECT * FROM kafka_set_next_offset('topic', 0::SMALLINT, 0, 100);
SELECT * FROM kafka_get_start_and_next_offset('topic', 0::SMALLINT, 0);
SELECT * FROM kafka_get_start_and_next_offset('topic', 1::SMALLINT, 0);
\c test2
SELECT * FROM kafka_get_start_and_next_offset('topic', 0::SMALLINT, 0);
\set ON_ERROR_STOP 0
SELECT * FROM kafka_set_next_offset('topic', 0::SMALLINT, 0, 101);
\set ON_ERROR_STOP 1
SELECT * FROM kafka_set_next_offset('topic', 0::SMALLINT, 100, 101);
\c Test1
SELECT * FROM kafka_get_start_and_next_offset('topic', 0::SMALLINT, 0);
SELECT * FROM kafka_offset_local;





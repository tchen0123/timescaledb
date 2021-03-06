#include <postgres.h>
#include <catalog/namespace.h>
#include <utils/lsyscache.h>
#include <miscadmin.h>
#include <commands/dbcommands.h>

#include "catalog.h"

static char *catalog_table_names[_MAX_CATALOG_TABLES] = {
	[HYPERTABLE] = HYPERTABLE_TABLE_NAME,
	[HYPERTABLE_REPLICA] = HYPERTABLE_REPLICA_TABLE_NAME,
	[DEFAULT_REPLICA_NODE] = DEFAULT_REPLICA_NODE_TABLE_NAME,
	[PARTITION] = PARTITION_TABLE_NAME,
	[PARTITION_EPOCH] = PARTITION_EPOCH_TABLE_NAME,
	[CHUNK] = CHUNK_TABLE_NAME,
	[CHUNK_REPLICA_NODE] = CHUNK_REPLICA_NODE_TABLE_NAME,
};

typedef struct TableIndexDef
{
	size_t		length;
	char	  **names;
} TableIndexDef;

const static TableIndexDef catalog_table_index_definitions[_MAX_CATALOG_TABLES] = {
	[HYPERTABLE] = {
		.length = _MAX_HYPERTABLE_INDEX,
		.names = (char *[]) {
			[HYPERTABLE_ID_INDEX] = "hypertable_pkey",
			[HYPERTABLE_NAME_INDEX] = "hypertable_schema_name_table_name_key",
		}
	},
	[HYPERTABLE_REPLICA] = {
		.length = _MAX_HYPERTABLE_REPLICA_INDEX,
		.names = (char *[]) {
			[HYPERTABLE_REPLICA_HYPERTABLE_REPLICA_INDEX] = "hypertable_replica_pkey",
		}
	},
	[DEFAULT_REPLICA_NODE] = {
		.length = _MAX_DEFAULT_REPLICA_NODE_INDEX,
		.names = (char *[]) {
			[DEFAULT_REPLICA_NODE_DATABASE_HYPERTABLE_INDEX] = "default_replica_node_pkey",
		}
	},
	[PARTITION] = {
		.length = _MAX_PARTITION_INDEX,
		.names = (char *[]) {
			[PARTITION_ID_INDEX] = "partition_pkey",
			[PARTITION_PARTITION_EPOCH_ID_INDEX] = "partition_epoch_id_idx",
		}
	},
	[PARTITION_EPOCH] = {
		.length = _MAX_PARTITION_EPOCH_INDEX,
		.names = (char *[]) {
			[PARTITION_EPOCH_ID_INDEX] = "partition_epoch_pkey",
			[PARTITION_EPOCH_TIME_INDEX] = "partition_epoch_hypertable_id_start_time_end_time_idx",
		}
	},
	[CHUNK] = {
		.length = _MAX_CHUNK_INDEX,
		.names = (char *[]) {
			[CHUNK_ID_INDEX] = "chunk_pkey",
			[CHUNK_PARTITION_TIME_INDEX] = "chunk_partition_id_start_time_end_time_idx",
		}
	},
	[CHUNK_REPLICA_NODE] = {
		.length = _MAX_CHUNK_REPLICA_NODE_INDEX,
		.names = (char *[]) {
			[CHUNK_REPLICA_NODE_ID_INDEX] = "chunk_replica_node_pkey",
		}
	},
};


/* Catalog information for the current database. Should probably be invalidated
 * if the extension is unloaded for the database. */
static Catalog catalog = {
	.database_id = InvalidOid,
};

Catalog *
catalog_get(void)
{
	int			i;

	if (MyDatabaseId == InvalidOid)
		elog(ERROR, "Invalid database ID");

	
	if (MyDatabaseId == catalog.database_id)
		return &catalog;

	memset(&catalog, 0, sizeof(Catalog));
	catalog.database_id = MyDatabaseId;
	strncpy(catalog.database_name, get_database_name(MyDatabaseId), NAMEDATALEN);
	catalog.schema_id = get_namespace_oid(CATALOG_SCHEMA_NAME, false);

	if (catalog.schema_id == InvalidOid)
	{
		elog(ERROR, "Oid lookup failed for schema %s", CATALOG_SCHEMA_NAME);
	}

	for (i = 0; i < _MAX_CATALOG_TABLES; i++)
	{
		Oid			id;
		int			number_indexes,
					j;

		id = get_relname_relid(catalog_table_names[i], catalog.schema_id);

		if (id == InvalidOid)
		{
			elog(ERROR, "Oid lookup failed for table %s", catalog_table_names[i]);
		}

		catalog.tables[i].id = id;

		number_indexes = catalog_table_index_definitions[i].length;
		Assert(number_indexes <= _MAX_TABLE_INDEXES);

		for (j = 0; j < number_indexes; j++)
		{
			id = get_relname_relid(catalog_table_index_definitions[i].names[j], catalog.schema_id);
			if (id == InvalidOid)
			{
				elog(ERROR, "Oid lookup failed for table index %s", catalog_table_index_definitions[i].names[j]);
			}

			catalog.tables[i].index_ids[j] = id;
		}

		catalog.tables[i].name = catalog_table_names[i];
	}

	return &catalog;
}

<?php
	require_once('b/config.php');
	require_once('b/common.php');

	function update_hof($date, $lid)
	{
		$rows = db_query(sprintf("select name, points, countries from xcl_prev_players where lid = %d order by points desc, pid limit 10", $lid));
		$rank = 1;
		while ($row = mysql_fetch_assoc($rows))
			db_query(sprintf("insert into xcl_hof (date, lid, rank, name, points, countries) values ('%s', %d, %d, '%s', %d, %d)",
				$date, $lid, $rank++, addslashes($row['name']), $row['points'], $row['countries']));
	}

	db_connect();
	$row = db_query_first("select mtime from xcl_games order by gid limit 1");
	$year = gmdate('Y', $row['mtime']);
	$month = gmdate('m', $row['mtime']) - 1;
	if (!$month)
	{
		$month += 12;
		$year--;
	}
	$date = sprintf('%04d%02d00', $year, $month);
	$row = db_query_first(sprintf("select count(*) c from xcl_hof where date = '%s'", $date));
	if ($row['c'])
	{
		if ($_GET['force'] != 1)
			die('hof exists already');
		db_query(sprintf("delete from xcl_hof where date = '%s'", $date));
	}
	db_query("update xcl_prev_players inner join bl using (name) set points = 0 where lid & 1 and points");
	update_hof($date, 1);
	update_hof($date, 2);
	update_hof($date, 3);
	update_hof($date, 4);
	update_hof($date, 7);
	update_hof($date, 8);
	db_query("insert ignore into xcl_high_scores (name, points, rank) select name, points, rank from xcl_prev_players inner join xcl_prev_players_rank using (pid) where lid = 3 and points");
	db_query("update xcl_high_scores h inner join xcl_prev_players p using (name) inner join xcl_prev_players_rank pr using (pid) set h.points = greatest(h.points, p.points), h.rank = least(h.rank, pr.rank) where lid = 3");
?>
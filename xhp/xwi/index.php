<link rel=stylesheet href="/xcl.css">
<a href="?a=create">Create</a> | <a href="?">Home</a> | <a href="?a=invite">Invite</a> | <a href="?a=join">Join</a> | <a href="?a=kick">Kick</a> | <a href="?a=leave">Leave</a> | <a href="?a=delete_nick">Delete nick</a>
<hr>
<?php
	function apgar_encode($v)
	{
		$a = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
		$w = "";
		for ($i = 0; $i < 8; $i++)
		{
			$b = ord($v[$i]);
			$c = ord($v[8 - $i]);
			$w .= $a[($b & 1 ? $b << ($b & 1) & $c : $b ^ $c) & 0x3f];
		}
		return $w;
	}

	function new_security_code()
	{
		$v = "";
		$s = "0123456789ABCDEFGHIJKLMNOPQRSTUVXWXYZabcdefghijklmnopqrstuvxwxyz";
		for ($i = 0; $i < 16; $i++)
			$v .= $s[mt_rand(0, strlen($s) - 1)];
		return $v;
	}

	function get_clan($cid)
	{
		return mysql_fetch_array(db_query(sprintf("select * from xwi_clans where cid = %d", $cid)));
	}

	function get_player($name)
	{
		return mysql_fetch_array(db_query(sprintf("select * from xwi_players where name = \"%s\"", AddSlashes($name))));
	}

	function get_player2($name, $pass)
	{
		return mysql_fetch_array(db_query(sprintf("select * from xwi_players where name = \"%s\" and pass = md5(\"%s\")", AddSlashes($name), apgar_encode($pass))));
	}

	require("../xcc_common.php");

	ob_start(ob_gzhandler);
	db_connect();

	$name = $_POST[name];
	$pass = $_POST[pass];

	switch ($_GET[a])
	{
	case "edit":
		{
			$cid = $_POST[cid] ? $_POST[cid] : $_GET[cid];
			if (isset($pass))
			{
				if ($clan = mysql_fetch_array(db_query(sprintf("select * from xwi_clans where cid = %d and pass = md5(\"%s\")", $cid, AddSlashes($pass)))))
				{
					$icq = $_POST[icq];
					$mail = $_POST[mail];
					$msn = $_POST[msn];
					$site = $_POST[site];
					db_query(sprintf("update xwi_clans set icq = %d, mail = \"%s\", msn = \"%s\", site = \"%s\" where cid = %d", $icq, AddSlashes($mail), AddSlashes($msn), AddSlashes($site), $clan[cid]));
				}
				else
					echo("Wrong clan pass<hr>");
			}
			$clan = get_clan($cid);
			?>
			<table>
				<form action="?a=edit" method=post>
					<input type=hidden name=cid value=<?php printf($cid) ?>>
					<tr>
						<td align=right>Clan pass:
						<td><input type=password name=pass>
					<tr>
						<td align=right>ICQ:
						<td><input type=text name=icq maxlength=15 value="<?php if ($clan[icq]) echo(htmlspecialchars($clan[icq]))?>">
					<tr>
						<td align=right>Mail:
						<td><input type=text name=mail maxlength=63 value="<?php echo(htmlspecialchars($clan[mail]))?>">
					<tr>
						<td align=right>MSN:
						<td><input type=text name=msn maxlength=63 value="<?php echo(htmlspecialchars($clan[msn]))?>">
					<tr>
						<td align=right>Site:
						<td><input type=text name=site maxlength=127 value="<?php echo(htmlspecialchars($clan[site]))?>">
					<tr>
						<td>
						<td><input type=submit value="Edit">
					</tr>
				</form>
			</table>
			<?php
		}
		break;
	case "create":
		{
			$cname = $_POST[cname];
			$icq = $_POST[icq];
			$mail = $_POST[mail];
			$msn = $_POST[msn];
			$site = $_POST[site];
			if (!$site)
				$site = "http://";
			if ($name || $pass || $cname)
			{
				if (!eregi("^[0-9a-z]{2,6}$", $cname))
					echo("Invalid clan name");
				else if ($player = get_player2($name, $pass))
				{
					if ($player[cid])
					{
						$clan = get_clan($player[cid]);
						printf("Player %s is already in clan %s", $player[name], $clan[name]);
					}
					else if ($clan = mysql_fetch_array(db_query(sprintf("select name from xwi_clans where name = \"%s\"", AddSlashes($cname)))))
						printf("Clan %s already exists", $clan[name]);
					else
					{
						do
						{
							$cpass = new_security_code();
							$results = db_query(sprintf("select count(*) from xwi_clans where pass = md5(\"%s\")", $cpass));
							$result = mysql_fetch_array($results);
						}
						while ($result[0]);
						db_query(sprintf("insert into xwi_clans (name, pass, icq, mail, msn, site, ctime) values (lcase(\"%s\"), md5(\"%s\"), %d, lcase(\"%s\"), lcase(\"%s\"), lcase(\"%s\"), NULL)", AddSlashes($cname), $cpass, $icq, AddSlashes($mail), AddSlashes($msn), AddSlashes($site)));
						$cid = mysql_insert_id();
						db_query(sprintf("update xwi_players set cid = %d where pid = %d", $cid, $player[pid]));
						$clan = get_clan($cid);
						printf("Player %s created clan %s<br>", $player[name], $clan[name]);
						printf("The clan admin pass is %s", $cpass);
						if (strlen($mail))
							mail($mail, sprintf("XWI Clan Manager: Clan %s created", $clan[name]), sprintf("Player %s created clan %s with admin pass %s from IP address %s", $player[name], $clan[name], $cpass, $_SERVER[REMOTE_ADDR]));
					}
				}
				else
					echo("Wrong name/pass combo");
				echo("<hr>");
			}
			?>
			<table>
				<form action="?a=create" method=post>
					<tr>
						<td align=right>Name:
						<td><input type=text name=name value="<?php echo(htmlspecialchars($name))?>">
					<tr>
						<td align=right>Pass:
						<td><input type=password name=pass maxlength=8>
					<tr>
						<td align=right>Clan name:
						<td><input type=text name=cname maxlength=6 value="<?php echo(htmlspecialchars($cname))?>">
					<tr>
						<td align=right>ICQ:
						<td><input type=text name=icq maxlength=15 value="<?php echo(htmlspecialchars($icq))?>">
					<tr>
						<td align=right>Mail:
						<td><input type=text name=mail maxlength=63 value="<?php echo(htmlspecialchars($mail))?>">
					<tr>
						<td align=right>MSN:
						<td><input type=text name=msn maxlength=63 value="<?php echo(htmlspecialchars($msn))?>">
					<tr>
						<td align=right>Site:
						<td><input type=text name=site maxlength=127 value="<?php echo(htmlspecialchars($site))?>">
					<tr>
						<td>
						<td><input type=submit value="Create">
					</tr>
				</form>
			</table>
			<?php
		}
		break;
	case "delete_nick":
		{
			if ($name || $pass)
			{
				if ($player = get_player2($name, $pass))
				{
					$results = db_query(sprintf("select count(*) from xcl_players where lid & 1 and name = \"%s\"", $name));
					$result = mysql_fetch_array($results);
					if ($result[0])
						printf("Player %s is already in ladder", $player[name]);
					else
					{
						$results = db_query(sprintf("select to_days(now()) - to_days(ctime) from xwi_players where pid = %d", $player[pid]));
						$result = mysql_fetch_array($results);
						if ($result[0] < 32)
							printf("Only players that were created more than 32 days ago can be deleted. Player %s was created %d days ago", $player[name], $result[0]);
						else
						{
							db_query(sprintf("update xwi_players set flags = flags | 2 where pid = %d", $player[pid]));
							printf("Player %s has been deleted", $player[name]);
						}
					}
				}
				else
					echo("Wrong name/pass combo");
				echo("<hr>");
			}
			?>
			<table>
				<form action="?a=delete_nick" method=post>
					<tr>
						<td align=right>Name:
						<td><input type=text name=name value="<?php echo(htmlspecialchars($name))?>">
					<tr>
						<td align=right>Pass:
						<td><input type=password name=pass>
					<tr>
						<td>
						<td><input type=submit value="Delete">
					</tr>
				</form>
			</table>
			<?php
		}
		break;
	case "invite":
		{
			if ($name || $pass)
			{
				if ($player = get_player($name))
				{
					if ($player[cid])
					{
						$clan = get_clan($player[cid]);
						printf("Player %s is already in clan %s", $player[name], $clan[name]);
					}
					else if ($clan = mysql_fetch_array(db_query(sprintf("select * from xwi_clans where pass = md5(\"%s\")", AddSlashes($pass)))))
					{
						$result = mysql_fetch_array(db_query(sprintf("select count(*) from xwi_clan_invites where cid = %d", $clan[cid])));
						do
						{
							$cpass = new_security_code();
							$results = db_query(sprintf("select count(*) from xwi_clan_invites where pass = md5(\"%s\")", $cpass));
							$result = mysql_fetch_array($results);
						}
						while ($result[0]);
						if ($result[0] < 10)
							db_query(sprintf("insert into xwi_clan_invites (pid, cid, pass) values (%d, %d, md5(\"%s\"))", $player[pid], $clan[cid], $cpass));
						printf("Player %s may join %s with pass %s", $player[name], $clan[name], $cpass);
					}
					else
						echo("Wrong clan pass");
				}
				else
					echo("Unknown player");
				echo("<hr>");
			}
			?>
			<table>
				<form action="?a=invite" method=post>
					<tr>
						<td align=right>Name:
						<td><input type=text name=name value="<?php echo(htmlspecialchars($name))?>">
					<tr>
						<td align=right>Clan admin pass:
						<td><input type=password name=pass>
					<tr>
						<td>
						<td><input type=submit value="Invite">
					</tr>
				</form>
			</table>
			<?php
		}
		break;
	case "join":
		{
			$cpass = $_POST[cpass];
			if ($name || $pass || $cpass)
			{
				if ($player = get_player2($name, $pass))
				{
					db_query("delete from xwi_clan_invites where unix_timestamp(now()) - unix_timestamp(ctime) > 3 * 24 * 60 * 60");
					if ($player[cid])
					{
						$clan = get_clan($player[cid]);
						printf("Player %s is already in clan %s", $player[name], $clan[name]);
					}
					else if ($clan_invite = mysql_fetch_array(db_query(sprintf("select * from xwi_clan_invites where pass = md5(\"%s\")", AddSlashes($cpass)))))
					{
						db_query(sprintf("delete from xwi_clan_invites where pid = %d and cid = %d", $player[pid], $clan_invite[cid]));
						db_query(sprintf("update xwi_players set cid = %d where pid = %d", $clan_invite[cid], $player[pid]));
						$clan = get_clan($clan_invite[cid]);
						printf("Player %s joined clan %s<br>", $player[name], $clan[name]);
					}
					else
						echo("Wrong clan pass");
				}
				else
					echo("Wrong name/pass combo");
				echo("<hr>");
			}
			?>
			<table>
				<form action="?a=join" method=post>
					<tr>
						<td align=right>Name:
						<td><input type=text name=name value="<?php echo(htmlspecialchars($name))?>">
					<tr>
						<td align=right>Pass:
						<td><input type=password name=pass>
					<tr>
						<td align=right>Clan join pass:
						<td><input type=password name=cpass>
					<tr>
						<td>
						<td><input type=submit value="Join">
					</tr>
				</form>
			</table>
			<?php
		}
		break;
	case "kick":
		{
			if ($name || $pass)
			{
				if ($player = get_player($name))
				{
					if ($player[cid])
					{
						if ($clan = mysql_fetch_array(db_query(sprintf("select * from xwi_clans where pass = md5(\"%s\")", AddSlashes($pass)))))
						{
							if ($player[cid] == $clan[cid])
							{
								db_query(sprintf("update xwi_players set cid = 0 where pid = %d", $player[pid]));
								printf("Player %s left clan %s", $player[name], $clan[name]);
								$result = mysql_fetch_array(db_query(sprintf("select count(*) from xwi_players where cid = %d", $player[cid])));
								if (!$result[0])
									db_query(sprintf("delete from xwi_clans where cid = %d", $player[cid]));
							}
							else
								printf("Player %s is not in clan %s", $player[name], $clan[name]);
						}
						else
							echo("Wrong clan pass");
					}
					else
						printf("Player %s is not in a clan", $player[name]);
				}
				else
					echo("Unknown player");
				echo("<hr>");
			}
			?>
			<table>
				<form action="?a=kick" method=post>
					<tr>
						<td align=right>Name:
						<td><input type=text name=name value="<?php echo(htmlspecialchars($name))?>">
					<tr>
						<td align=right>Clan admin pass:
						<td><input type=password name=pass>
					<tr>
						<td>
						<td><input type=submit value="Kick">
					</tr>
				</form>
			</table>
			<?php
		}
		break;
	case "leave":
		{
			if ($name || $pass)
			{
				if ($player = get_player2($name, $pass))
				{
					if ($player[cid])
					{
						db_query(sprintf("update xwi_players set cid = 0 where pid = %d", $player[pid]));
						$clan = get_clan($player[cid]);
						printf("Player %s left clan %s", $player[name], $clan[name]);
						$result = mysql_fetch_array(db_query(sprintf("select count(*) from xwi_players where cid = %d", $player[cid])));
						if (!$result[0])
							db_query(sprintf("delete from xwi_clans where cid = %d", $player[cid]));
					}
					else
						printf("Player %s is not in a clan", $player[name]);
				}
				else
					echo("Wrong name/pass combo");
				echo("<hr>");
			}
			?>
			<table>
				<form action="?a=leave" method=post>
					<tr>
						<td align=right>Name:
						<td><input type=text name=name value="<?php echo(htmlspecialchars($name))?>">
					<tr>
						<td align=right>Pass:
						<td><input type=password name=pass>
					<tr>
						<td>
						<td><input type=submit value="Leave">
					</tr>
				</form>
			</table>
			<?php
		}
		break;
	default:
		$cid = $_GET[cid];
		if ($cid && $clan = get_clan($cid))
		{
			echo("<table>");
			printf("<tr><td align=right>Name:<td>%s", $clan[name]);
			if ($clan[icq])
				printf("<tr><td align=right>ICQ:<td><a href=\"http://wwp.icq.com/%d\"><img src=\"http://wwp.icq.com/scripts/online.dll?icq=%d&img=2\"></a>", $clan[icq], $clan[icq]);
			printf("<tr><td align=right>Mail:<td><a href=\"mailto:%s\">%s</a>", htmlspecialchars($clan[mail]), htmlspecialchars($clan[mail]));
			if ($clan[msn])
				printf("<tr><td align=right>MSN:<td><a href=\"mailto:%s\">%s</a>", htmlspecialchars($clan[msn]), htmlspecialchars($clan[msn]));
			if ($clan[site])
			{
				if (!strstr($clan[site], "://"))
					$clan[site] = "http://" . $clan[site];
				printf("<tr><td align=right>Site:<td><a href=\"%s\">%s</a>", htmlspecialchars($clan[site]), htmlspecialchars($clan[site]));
			}
			printf("<tr><td><td><a href=\"?a=edit&cid=%d\">Edit</a>", $clan[cid]);
			$results = db_query(sprintf("select * from xwi_players where cid = %d order by name", $cid));
			echo("</table><hr><table>");
			while ($result = mysql_fetch_array($results))
				printf("<tr><td><a href=\"?pid=%d\">%s</a>", $result[pid], $result[name]);
			echo("</table>");
		}
		else
		{
			?>
			<table>
				<form action="?" method=get>
					<tr>
						<td><input type=text name=text>
						<td><input type=submit value="Search">
				</form>
			</table>
			<hr>
			<?php
			if ($_GET[text])
				$results = db_query(sprintf("select xwi_clans.*, count(xwi_players.pid) size from xwi_clans left join xwi_players using (cid) where xwi_clans.name like '%s' or xwi_players.name like '%s' group by name order by size desc", $_GET[text], $_GET[text]));
			else
				$results = db_query("select xwi_clans.*, count(xwi_players.pid) size from xwi_clans left join xwi_players using (cid) group by name order by size desc");
			echo("<table>");
			while ($result = mysql_fetch_array($results))
				printf("<tr><td><a href=\"?cid=%d\">%s</a><td align=right>%d", $result[cid], $result[name], $result[size]);
			echo("</table>");
		}
	}
?>
<hr>
<a href="?a=create">Create</a> | <a href="?">Home</a> | <a href="?a=invite">Invite</a> | <a href="?a=join">Join</a> | <a href="?a=kick">Kick</a> | <a href="?a=leave">Leave</a> | <a href="?a=delete_nick">Delete nick</a>
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

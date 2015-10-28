<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
		<title>迷子検索ぺージ</title>
	</head>
	<body><?php
		if(empty($_POST["color1"]) || empty($_POST["color2"]) || empty($_POST["height"])){
				echo "必要な情報を選択してください。"."<hr>";
				echo "<a href='cloth.html'>はじめに戻る</a>";
				exit();
		}
		
		echo "<div align='center' ><font size='6'>迷子探しサービス</font></div>"."<br>"."<hr>";

		try{
				$pdo= new PDO("mysql:host=localhost; dbname=nemo","root","root",
					array(
						PDO::MYSQL_ATTR_INIT_COMMAND => "SET CHARACTER SET 'utf8'"
				));
			}catch(PDOException $e){
				die($e->getMessage());
			}
			$h=intval($_POST["height"]);
			if($h==40){
			$sql="select id,picture,time from children where find=1 and height<=".$h." and color1='".$_POST["color1"]."' and color2='".$_POST["color2"]."'";
			}elseif($h==130){
			$sql="select id,picture,time from children where find=1 and height>=".$h." and color1='".$_POST["color1"]."' and color2='".$_POST["color2"]."'";
			}else{
			$a=$h+15;
			$b=$h-15;
			$sql="select id,picture,time from children where find=1 and height<=".$a." and height>=".$b." and color1='".$_POST["color1"]."' and color2='".$_POST["color2"]."'";
			}
			$result=$pdo->query($sql);
			$i=0;
			while($row=$result->fetch(PDO::FETCH_ASSOC)){
				echo "<table border='1'><tr>";
					echo "<td><img src='".$row["picture"]."' width=50 height=50></td>";
					echo "<td>".$row["time"]."</td></tr>";
					echo "<form action='find.php' method='post'>";
						echo "<input type='hidden' name='id' value='".$row["id"]."'><input type='submit' value='見つけた'>";
					echo "</form>";
				echo "</table>";
				echo "<br><br>";
				$i++;
			}
			if($i==0){
				echo "検索条件に該当するものはございませんでした。";
			}
	?>
	<hr>
	<div align="right"><a href="cloth.html">検索画面へ</a></div>
	</body>
</html>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
		<title>迷子検索ぺージ</title>
		
	</head>
	<body><?php
		if(empty($_POST["color1"]) || empty($_POST["color2"])){
				echo "必要な情報を全て選択してください。"."<hr>";
				echo "<a href='cloth.html'>前へ戻る</a>";
				exit();
		}
		
		echo "<div align='center' ><font size='6'>迷子探しサービス</font></div>"."<br>"."<hr>";
		echo "<div align='center'>";
		echo "<form method='POST' action='search&list.php'>";
		echo	"お子様の身長はどのくらい？（上）"."<br>"."<br>";
		echo	"<p align='left' style='margin-left :630px'>";
		echo	"<input type='radio' name='height' value='40'>40cm以下"."<br>";
		echo	"<input type='radio' name='height' value='55'>40cm～70cm"."<br>";
		echo	"<input type='radio' name='height' value='85'>70cm～100cm"."<br>";
		echo	"<input type='radio' name='height' value='115'>100cm～130cm"."<br>";
		echo	"<input type='radio' name='height' value='130'>130cm以上"."<br>"."<hr>";
		echo	"</p>";
		echo	"<p><input type='hidden' name='color1' value=".$_POST["color1"]."></p>";
		echo	"<p><input type='hidden' name='color2' value=".$_POST["color2"]."></p>";
		echo	"<input type='submit' value='次へ'>";
		echo "</form></div>";
	?></body>
</html>
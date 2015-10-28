<?php
	try{
		$pdo= new PDO("mysql:host=localhost; dbname=nemo","root","root",
			array(
				PDO::MYSQL_ATTR_INIT_COMMAND => "SET CHARACTER SET 'utf8'"
		));
	}catch(PDOException $e){
		die($e->getMessage());
	}
	$sql="update children set find=0 where id=".intval($_POST["id"]);
	$result=$pdo->query($sql);
	while($row=$result->fetch(PDO::FETCH_ASSOC)){
				
	}
	echo "データを更新しました。<br><br>";
	echo "<a href='cloth.html'>検索画面へ</a>";
?>
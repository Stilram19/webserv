

<?php

$var = "omar"


?>

<html>

<body>
    <?= readfile("test.c") ?>
    <?= "\n" ?>
    <?php for ($i = 0; $i < 10; $i++) { ?>
        <h1> <?= $var ?> </h1>
    <?php } ?>
</body>

</html>
import java.io.File

fun main() {
    val text = File("id_start.compress.txt").readText().iterator()

    val result = mutableListOf<String>()
    var charCount = 0
    val buffer = StringBuilder()

    while (text.hasNext()) {
        val char = text.next()
        charCount++
        if (charCount < 950 || char != '|') {
            buffer.append(char)
        } else {
            result.add(buffer.toString())
            buffer.clear()
            charCount = 0
        }
    }

    if (charCount != 0) {
        result.add(buffer.toString())
        buffer.clear()
    }

    val output = result.mapIndexed { index, s -> "ID_START_${index} $s" }.toMutableList()
    output.add(
            "ID_START " + List(result.size) { index -> "{ID_START_${index}}" }.joinToString("|")
    )

    File("id_start.split.txt").writeText(output.joinToString("\n\n"))

}
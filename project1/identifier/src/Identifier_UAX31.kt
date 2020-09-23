import java.io.File

fun main() {
    val codepoints = mutableListOf<Int>()

    File("id_start.txt").readLines().forEach { line ->
        if (!line.contains('-')) {
            val codepoint = line.removePrefix("\\u").toInt(16)
            codepoints.add(codepoint)
        } else {
            val (start, end) = line.split('-')
            val startCodepoint = start.removePrefix("\\u").toInt(16)
            val endCodepoint = end.removePrefix("\\u").toInt(16)

            for (i in startCodepoint..endCodepoint) {
                codepoints.add(i)
            }
        }
    }

    println(codepoints.size)

    codepoints.map { StringBuilder().appendCodePoint(it).toString() }
            .map { it.toByteArray(Charsets.UTF_8) }
            .map { arr -> arr.map { String.format("%02X", it) }.joinToString("") { "\\x${it}" } }
            .joinToString("|")
            .let {
                File("id_start.output.txt").writeText(it)
            }

}



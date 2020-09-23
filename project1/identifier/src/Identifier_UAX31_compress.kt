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

    val bytes = codepoints.map { StringBuilder().appendCodePoint(it).toString() }
            .map { it.toByteArray(Charsets.UTF_8) }


    val result = mutableListOf<String>()

    var start = bytes.first()
    var end = bytes.first()

    for (current in bytes.asSequence().drop(1)) {
        if (canMerge(current, end)) {
            end = current
        } else {
            result.add(merge(start, end))

            start = current
            end = current
        }
    }
    result.add(merge(start, end))

    File("id_start.compress.txt").writeText(result.joinToString("|"))

}


@OptIn(ExperimentalUnsignedTypes::class)
fun canMerge(arr: ByteArray, last: ByteArray): Boolean {
    if (arr.size != last.size) {
        return false
    }
    val size = arr.size
    for (i in 0 until (size - 1)) {
        if (arr[i] != last[i]) {
            return false
        }
    }

    if (arr.last().toUByte().toInt() == last.last().toUByte().toInt() + 1) {
        return true
    }
    return false
}


fun merge(start: ByteArray, end: ByteArray): String {
    if (start.contentEquals(end)) {
        return start.map { String.format("%02X", it) }.joinToString("") { "\\x${it}" }
    }
    val from = start.last().let { String.format("%02X", it) }.let { "\\x$it" }
    val to = end.last().let { String.format("%02X", it) }.let { "\\x$it" }
    val prefix = start.dropLast(1).map { String.format("%02X", it) }.joinToString("") { "\\x${it}" }
    return "$prefix[$from-$to]"
}



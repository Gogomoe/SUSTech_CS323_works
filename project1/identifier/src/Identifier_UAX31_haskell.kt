import java.io.File

fun main() {

    val result = mutableListOf<String>()


    File("id_start.txt").readLines().forEachIndexed { index, line ->
        if (!line.contains('-')) {
            val codepoint = line.removePrefix("\\u").toInt(16)
            val utf8 = StringBuilder().appendCodePoint(codepoint).toString().toByteArray(Charsets.UTF_8)
            val hex = utf8.map { String.format("%02X", it) }.joinToString("") { "\\x${it}" }
            result.add(hex)
            println("$index: $hex")
        } else {
            val (start, end) = line.split('-')
            val startCodepoint = start.removePrefix("\\u").toInt(16)
            val endCodepoint = end.removePrefix("\\u").toInt(16)

            File("Main.hs").writeText("""
                import Unicode

                main = print $ urToRegU8 $startCodepoint $endCodepoint
            """.trimIndent())

            val process = ProcessBuilder("C:\\ProgramData\\chocolatey\\lib\\ghc\\tools\\ghc-8.10.2\\bin\\runhaskell.exe", ".\\Main.hs").start()
            process.waitFor()
            val str = String(process.inputStream.readAllBytes()).trim()
            result.add(str)
            println("$index: $str")
        }
        File("id_start.hs.txt").writeText(result.joinToString("|"))
    }


}



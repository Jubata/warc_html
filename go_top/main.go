package main

import (
	"bufio"
	"encoding/csv"
	"io"
	"log"
	"os"
	"sort"
	"strconv"
	"strings"
)

//URLStat optional article
type URLStat struct {
	totalFields           int
	countUrls             int
	bestURL               string
	bestMeanFieldsPerPage int
}

func main() {
	var stdin *os.File
	if len(os.Args) == 2 {
		var err error
		stdin, err = os.Open(os.Args[1])
		if err != nil {
			log.Fatal(err)
		}
	} else {
		stdin = os.Stdin
	}

	m := make(map[string]URLStat)

	reader := csv.NewReader(bufio.NewReader(stdin))
	for {
		record, err := reader.Read()
		if err == io.EOF {
			break
		}
		if err != nil {
			log.Fatal(err)
		}

		totalFields, err := strconv.Atoi(record[1])
		if err != nil {
			log.Fatal(err)
		}
		countUrls, err := strconv.Atoi(record[2])
		if err != nil {
			log.Fatal(err)
		}
		bestMeanFieldsPerPage, err := strconv.Atoi(record[4])
		if err != nil {
			log.Fatal(err)
		}

		rstat := URLStat{
			totalFields:           totalFields,
			countUrls:             countUrls,
			bestURL:               record[3],
			bestMeanFieldsPerPage: bestMeanFieldsPerPage,
		}
		if rstat.bestMeanFieldsPerPage > 10 {
			rstat.totalFields = rstat.countUrls * 10
		}
		url := record[0]
		url = url[:strings.IndexByte(url, '#')]
		stat := m[url]

		if rstat.totalFields > stat.totalFields {
			m[url] = rstat
		}
	}

	v := make([]URLStat, 0, len(m))

	for _, value := range m {
		v = append(v, value)
	}
	sort.Slice(v, func(i, j int) bool { return v[i].totalFields > v[j].totalFields })

	w := csv.NewWriter(os.Stdout)
	for _, element := range v {
		strs := []string{
			element.bestURL,
			strconv.Itoa(element.totalFields),
			strconv.Itoa(element.bestMeanFieldsPerPage),
			strconv.Itoa(element.countUrls),
		}
		if err := w.Write(strs); err != nil {
			log.Fatalln("error writing record to csv:", err)
		}
	}

	w.Flush()

}

package main

import (
	"log"
	"os"
	"sort"

	stats "../go_stats"
)

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

	m, err := stats.ReadStats(stdin)
	if err != nil {
		log.Fatal(err)
	}

	v := make([]stats.URLStat, 0, len(m))

	for _, value := range m {
		v = append(v, value)
	}
	sort.Slice(v, func(i, j int) bool { return v[i].TotalFields > v[j].TotalFields })

	stats.Write(v, os.Stdout)
}

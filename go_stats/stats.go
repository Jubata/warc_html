package stats

import (
	"bufio"
	"encoding/csv"
	"io"
	"net/url"
	"strconv"
	"strings"
)

//URLStat ...
type URLStat struct {
	TotalFields           int
	CountUrls             int
	BestURL               string
	BestMeanFieldsPerPage int
}

//Write writes url stats to csv
func Write(stats []URLStat, wr io.Writer) error {
	w := csv.NewWriter(wr)
	for _, element := range stats {
		url, err := url.Parse(element.BestURL)
		if err != nil {
			return err
		}

		strs := []string{
			url.Hostname() + "#",
			strconv.Itoa(element.TotalFields),
			strconv.Itoa(element.CountUrls),
			element.BestURL,
			strconv.Itoa(element.BestMeanFieldsPerPage),
		}
		if err := w.Write(strs); err != nil {
			return err
		}
	}
	w.Flush()
	return nil
}

//ReadStats read url stats from stream
func ReadStats(rd io.Reader) (map[string]URLStat, error) {
	m := make(map[string]URLStat)

	reader := csv.NewReader(bufio.NewReader(rd))
	for {
		record, err := reader.Read()
		if err == io.EOF {
			break
		}
		if err != nil {
			return nil, err
		}

		TotalFields, err := strconv.Atoi(record[1])
		if err != nil {
			return nil, err
		}
		CountUrls, err := strconv.Atoi(record[2])
		if err != nil {
			return nil, err
		}
		BestMeanFieldsPerPage, err := strconv.Atoi(record[4])
		if err != nil {
			return nil, err
		}

		rstat := URLStat{
			TotalFields:           TotalFields,
			CountUrls:             CountUrls,
			BestURL:               record[3],
			BestMeanFieldsPerPage: BestMeanFieldsPerPage,
		}
		if rstat.BestMeanFieldsPerPage > 10 {
			rstat.TotalFields = rstat.CountUrls * 10
		}
		url := record[0]
		url = url[:strings.IndexByte(url, '#')]
		stat := m[url]

		if rstat.TotalFields > stat.TotalFields {
			m[url] = rstat
		}
	}

	return m, nil
}

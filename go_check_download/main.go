package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"os"
	"os/exec"
	"strconv"
	"time"

	stats "../go_stats"
	scribble "github.com/nanobox-io/golang-scribble"
)

type URLBody struct {
	BestURL               string
	HtmlBody              []byte
	BestMeanFieldsPerPage int
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

	m, err := stats.ReadStats(stdin)
	if err != nil {
		log.Fatal(err)
	}

	db, err := scribble.New("./db", nil)
	if err != nil {
		log.Fatal(err)
	}

	//get downloaded items list
	records, err := db.ReadAll("urlBody")
	if err != nil {
		log.Fatal(err)
	}
	have := make(map[string]int)
	i := 0
	for _, f := range records {
		urlRecord := URLBody{}
		if err := json.Unmarshal([]byte(f), &urlRecord); err != nil {
			log.Fatal(err)
		}
		have[urlRecord.BestURL] = 1
		i++
	}

	for _, stat := range m {
		if _, ok := have[stat.BestURL]; ok {
			continue //skip already downloaded items
		}

		var netTransport = &http.Transport{
			Dial: (&net.Dialer{
				Timeout: 5 * time.Second,
			}).Dial,
			TLSHandshakeTimeout: 5 * time.Second,
		}
		var netClient = &http.Client{
			Timeout:   time.Second * 30,
			Transport: netTransport,
		}

		resp, err := netClient.Get(stat.BestURL)
		if err != nil {
			continue
			// log.Fatal(err)
		}
		defer resp.Body.Close()
		body, err := ioutil.ReadAll(resp.Body)
		if err != nil {
			continue
			// log.Fatal(err)
		}

		urlRecord := URLBody{
			BestURL:               stat.BestURL,
			HtmlBody:              body,
			BestMeanFieldsPerPage: stat.BestMeanFieldsPerPage,
		}

		db.Write("urlBody", strconv.Itoa(i), urlRecord)
		i++
	}

	records, err = db.ReadAll("urlBody")
	if err != nil {
		log.Fatal(err)
	}

	for _, f := range records {
		urlRecord := URLBody{}
		if err := json.Unmarshal([]byte(f), &urlRecord); err != nil {
			log.Println(urlRecord.BestURL)
			log.Fatal(err)
		}

		cmd := exec.Command("./parse_html")
		cmd.Stdin = bytes.NewReader(urlRecord.HtmlBody)

		// stdin, err := cmd.StdinPipe()
		// if err != nil {
		// 	log.Fatal(err)
		// }
		stdout, err := cmd.StdoutPipe()
		if err != nil {
			log.Fatal(err)
		}
		err = cmd.Start()
		if err != nil {
			log.Fatal(err)
		}

		m, err := stats.ReadStats(stdout)
		if err != nil {
			log.Fatal(err)
		}

		cmd.Wait()

		maxBest := 0
		for _, v := range m {
			if v.BestMeanFieldsPerPage > maxBest {
				maxBest = v.BestMeanFieldsPerPage
			}
		}

		if maxBest >= urlRecord.BestMeanFieldsPerPage {
			fmt.Println(urlRecord.BestURL)
		}
	}
}

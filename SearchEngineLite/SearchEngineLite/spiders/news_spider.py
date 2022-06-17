import scrapy


class NewsSpider(scrapy.Spider):
    name = "news"

    start_urls = [
        'http://sh.people.com.cn/n2/2022/0605/c134768-35301044.html'
    ]

    def parse(self, response):
        for news in response.css('div.col.col-1.fl'):
            if news.css('h1::text').get() is not None:
                url = response.url
                title = news.css('h1::text').get()
                content = ' '.join(news.css('p::text').getall())
                filename = f'news.csv'
                res = bytes(url+",\""+title+"\",\""+content+"\",\n", encoding="utf8")
                with open(filename, 'ab') as f:
                    f.write(res)

        anchors_a = response.css('div.rm_relevant.cf.box_news a')
        yield from response.follow_all(anchors_a, callback=self.parse)



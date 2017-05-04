BEGIN {
	fpr["1"] = 16;fpr["2"] = 35;fpr["3"] = 49;fpr["4"] = 61;rpc["1"] = 5;rpc["2"] = 21;rpc["3"] = 46;rpc["4"] = 49;
	c = 1;
	for(i=1;i<5;i++) {
	
		for(j=1;j<5;j++) {
			cmd = "./ers 65 2100 3120 10 t2000.txt te2000.txt " fpr[i] " " rpc[j] " 10000 > 2T"fpr[i] "-" rpc[j]".txt";
			c++;
			system(cmd);
			print cmd "FINISHED";
		}
	}
}
{}
END {}

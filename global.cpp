#include "main.hpp"
#include <iomanip>
vector<int> res;
//#define BYPASSSIZE 500
//#define DONT_CACHE
Global::Global(){
  time=0.0;
  start_time=0.0;
  end_time=0.0;
  proc_size=0.0;
  table_stall_time={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  table_stall_count={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  cache_stall_time={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  cache_stall_count={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  dram_stall_time=0.0;
  dram_stall_count=0.0;
  res={0,0,0,0,0,0,0,0};
  tracefile="";
  input.clear();
  resfile="";
  output.clear();
  //decode procedure counter
  table_hit_cache_hit=0;
  table_hit_cache_miss=0;
  table_miss_decode_first_packet=0;
  dont_cache_decode_first_packet=0;
  dont_cache_decode_following_packet=0;
  //
  num_cache=0;
  num_prediction_miss=0;
  conf_cache.clear();
  num_eachcache.clear();
  num_decmod=0;
  size_queue=0;
  //delay
  delay_cache=0.0;
  delay_decode=0.0;
  delay_makehuff=0.0;
  delay_table=0.0;
  delay_dram=0.0;
  event_handler.clear();
  mod=nullptr;
  cache=nullptr;
  table=nullptr;
  num_packet=0;

  num_drop=0;
  num_hit=0;
  num_of_through=0;
  res_sec=1;
  nt=0;
  np=0;
  nd=0;
  nh=0;
  pcount=0;
  num_of_ReadedPacket=0;
}

Global::~Global(){
	delete[] mod;
	delete[] cache;
    delete[] table;
    delete dram;
}

void Global::readConf(int argc, char *argv[]){
  if(argc >= 2){
    istringstream is;
    string line, tag, val;
    ifstream conffile(argv[1], ifstream::in);
    if(!conffile.is_open()){
      cout << "Error: config file does not exist." << endl;
      exit(-1);
    }
    while(getline(conffile, line)){
      if(line.size() == 0){continue;}
      is.clear(), tag = "", val = "";
      is.str(line);
      is >> tag >> val;
      if(!tag.compare("TRACEFILE")){tracefile = val;}
      if(!tag.compare("RESFILE")){resfile = val;}
      if(!tag.compare("CACHENUMBER")){num_cache = (u_int)stoi(val);}
      if(!tag.compare("CACHEASSIGN")){
	for(unsigned int n=0; n<num_cache; n++){
	  u_int i = (u_int)stoi(val);
	  conf_cache.push_back(i);
	  num_eachcache[i].first = n;  //  そのサイズのキャッシュが最後に出てくるキャッシュ番号
	  num_eachcache[i].second += 1;    //  そのサイズのキャッシュの個数
	  is >> val;
	}
	if(num_eachcache.count(32) == 0){
	  cout << "Error: at least 1 cache is 32KB." << endl;
	  exit(-1);
	}
      }
      if(!tag.compare("CACHEDELAY")){delay_cache = stod(val);}
      if(!tag.compare("DECMODNUMBER")){num_decmod = (u_int) stoi(val);}
      if(!tag.compare("GZIPDECDELAY")){delay_decode = stod(val);}
      if(!tag.compare("MAKEHUFFDELAY")){delay_makehuff = stod(val);}
      if(!tag.compare("QUEUESIZE")){size_queue = (u_int) stoi(val);}
      if(!tag.compare("TABLEDELAY")){delay_table = stod(val);}
      if(!tag.compare("DRAMDELAY")){delay_dram = stod(val);}
    }
    conffile.close();
  }else{
    cout << "Error: the number of argument is inappropriate." << endl;
    exit(-1);
  }
}

void Global::showConf(){
    cout << "------------------CONFIG---------------------" << endl;
    cout << "TRACEFILE " << tracefile << endl;
    cout << "RESFILE " << resfile << endl;
    cout << "CACHENUMBER " << num_cache << endl;
    cout << "CACHEASSIGN ";
    map<u_int, pair<u_int, u_int> >::reverse_iterator it = num_eachcache.rbegin();
        while(it != num_eachcache.rend()){
            cout << it->first << "KB->" << it->second.second << " ";
            it++;
        }
    cout << endl;
    cout << "CACHEDELAY " << delay_cache << endl;
    cout << "DECMODNUMBER " << num_decmod << endl;
    cout << "GZIPDECDELAY " << delay_decode << endl;
    cout << "MAKEHUFFDELAY " << delay_makehuff << endl;
    cout << "QUEUESIZE " << size_queue << endl;
    cout << "TABLEDELAY " << delay_table << endl;
    cout << "DRAMDELAY " << delay_dram << endl;
}

void Global::initSim(){
    //  Decoding Moduleの初期化
    mod = new Decmod[num_decmod];
    //  Cacheの初期化
    cache = new Cache[num_cache];
    for(unsigned int n=0; n<num_cache; n++){
        cache[n].size = conf_cache[n];
        cache[n].num_entry = 32 / conf_cache[n];
	//Cacheごとのline数を計算(ex.32KB->1)
        if(cache[n].num_entry < 4){
            cache[n].num_line = 1;
        }else{
            cache[n].num_line = cache[n].num_entry / 4; // 4wayを想定
        }
    }
    //  Tableの初期化
    table = new Table[num_decmod];
    //  Dramの初期化
    dram = new Dram;
    //  PCAPの読み込み
    input.open(tracefile.c_str(), ifstream::in);
    if(!input.is_open()){
        cout << "Error: trace file does not exist." << endl;
        exit(-1);
    }
    //  出力ファイルの作成
    output.open(resfile.c_str(), ofstream::out);
    if(!output.is_open()){
        cout << "Error: result file cannot open." << endl;
        exit(-1);
    }
}

void Global::showResult(){
  cout << res_sec << " sec\t"<< num_packet - np << " packets\t" << num_drop - nd << " drops\t" << num_of_through - nt << " troughs\t" << ((double)(num_hit - nh)/((double)(num_packet - np)-(double)(num_of_through - nt))) << " hit_ratio" << endl;
	np = num_packet;
	nd = num_drop;
	nt = num_of_through;
	nh = num_hit;
}

void Global::reportResult(){
    cout << "------------------RESULT---------------------" << endl;
    cout << "# of Packets: " << num_packet << endl;
    //cout << "# of Flow: " << table->tag.size() << endl;
    cout << "# of Packet drops: " << num_drop << endl;
    cout << "# of don't cache: " << num_dontcache << endl;
    cout << "# of Read through: " << num_of_through << endl;
    cout << "# of prediction miss: " << num_prediction_miss << endl;
    cout << "# of Cache hits: " << num_hit << endl;
    cout << "Cache hit rate: " << (double)num_hit/cache_access_count << endl;
    cout << "Process time: " << end_time - start_time << "sec" << endl;
    cout << "Process size: " << proc_size/(1000*1000) << "MByte" << endl;
    cout << "Throughput: " << (proc_size * 8)/((end_time-start_time)*1000*1000) << "Mbps" << endl;
    cout << "------------------DEBUG----------------------" << endl;

    cout << "** Decode procedure **" << endl;
    cout << "# of table hit cache hit: " << table_hit_cache_hit << endl;
    cout << "# of table hit cache miss: " << table_hit_cache_miss << endl;
    cout << "# of table miss decode first packet: " << table_miss_decode_first_packet << endl;
    cout << "# of dont cache decode first packet: " << dont_cache_decode_first_packet << endl;
    cout << "# od dont cache decode following packet: " << dont_cache_decode_following_packet << endl;
    cout << endl;
    
    cout << "** Table info **" << endl;
    for(int i=0;i<8;i++){
      cout << "Table[" << i << "] stall count: " << table_stall_count[i] << endl;
	}
    for(int i=0;i<8;i++){
      cout << "Table[" << i << "] stall time: " << table_stall_time[i] << endl;
	}
    cout << endl;

    cout << "** Cache info **" << endl;
    for(int i=0;i<8;i++){
      cout << "# of Flow to Cache[" << i << "]: " << res[i] << endl;
    }
    for(int i=0;i<8;i++){
      cout << "Cache[" << i << "] stall count: " << cache_stall_count[i] << endl;
    }
    for(int i=0;i<8;i++){
      cout << "Cache[" << i << "] stall time: " << cache_stall_time[i] << endl;
    }
    cout << endl;

    cout << "** Dram info **" << endl;
    cout << "Dram request count: " << dram_request_count << endl;
    cout << "Dram stall count: " << dram_stall_count << endl;
    cout << "Dram stall time: " << dram_stall_time << endl;
    cout << "---------------------------------------------" << endl;

    cout << "** Check sum**"
    
    exit(1);
}

void Global::inputPacket(Packet p){
    string line;
    getline(input, line);
    if(input.eof()){reportResult();}    //  Simulation の終了
    num_packet++;
    next_p.id = num_packet;
    if(num_packet==10001){start_time=end_time;}
    istringstream is, ip;
    string sip, dip;
    is.str(line);
    is >> next_p.timestamp >> next_p.length >> sip >> dip >> next_p.protocol >> next_p.sport >> next_p.dport >> next_p.comp_len >> next_p.decomp_len >> next_p.comp_ratio >> next_p.stream_num >> next_p.flow_size;
    ip.str(sip);
    next_p.timestamp = double(num_packet)/(1000*1000*1000);
    if(num_packet>=10001){proc_size += next_p.length;}//***
    int i=0;
    while(getline(ip, sip, '.')){
        next_p.sip[i] = (unsigned char)atoi(sip.c_str());
        i++;
    }
    ip.clear();
    ip.str(dip);
    i=0;
    while(getline(ip, dip, '.')){
        next_p.dip[i] = (unsigned char)atoi(dip.c_str());
        i++;
    }
    stringstream ss;
    ss << next_p.sip[0] << next_p.sip[1] << next_p.sip[2] << next_p.sip[3] << next_p.dip[0] << next_p.dip[1] << next_p.dip[2] << next_p.dip[3] << next_p.sport << next_p.dport << next_p.protocol;
    next_p.flow_id = ss.str();
    num_of_ReadedPacket ++;

    CRC crc;
    crc.Align(next_p);
    //next_p.id = ++pcount; ??
    next_p.hash = crc.Calc(8, 13) % num_decmod;
    //  decmodのqueueに詰めるイベントを登録
    //cout << next_p.id << endl;
    Event e = pair<Func, Packet>(&Global::inputQueue, next_p);
    event_handler.insert(pair<double, Event>(next_p.timestamp, e));
}

void Global::inputQueue(Packet p){
  //cout << "debug:inputQ " << num_packet << endl;
  mod[p.hash].Enqueue(&p);
}

void Global::outputQueue(Packet p){
    mod[p.hash].Dequeue(&p);
}

void Global::accessTable(Packet p){
  Event e;
  //cout << "debug:AT" << num_packet << endl;
  if(p.timestamp >= table[p.hash].next_time){
    p.timestamp += delay_table;
    p.cache_num = table[p.hash].Access(p);
    if(p.cache_num == -1){//  first packet
      table[p.hash].UpdateReadedSize(p);//(add)0702
      if(table[p.hash].GetReadedSize(p) > p.flow_size - p.comp_len){
	p.length = table[p.hash].GetReadedSize(p) - p.flow_size + p.comp_len;
	table_miss_decode_first_packet ++;
	e = pair<Func, Packet>(&Global::decodeFirstPacket, p);
#ifdef DONT_CACHE
	if(p.comp_len <= BYPASSSIZE){
	  num_dontcache += 1;
	  dont_cache_decode_first_packet ++;
	  goto INSERT;
	}
#endif
      }else{
	e = pair<Func, Packet>(&Global::through, p);
      }
    }else{//  not first packet
      //cout << "nfp" << endl;
      table[p.hash].UpdateReadedSize(p);//(add)0702
#ifdef DONT_CACHE
      if(p.comp_len <= BYPASSSIZE){
	num_dontcache += 1;
	dont_cache_decode_following_packet ++;
	e = pair<Func, Packet>(&Global::accessDram, p); //11/28
	goto INSERT;//11/14
      }
#endif
      if(table[p.hash].GetReadedSize(p) > p.flow_size - p.comp_len){
	e = pair<Func, Packet>(&Global::accessCache, p);
      }else{
	e = pair<Func, Packet>(&Global::through, p);
      }
    }
  }else{
    if(num_packet >= 10001){
      table_stall_count[p.hash] += 1; //ストール回数のインクリ
      if(table[p.hash].next_time - p.timestamp >= delay_table){
	table_stall_time[p.hash] += delay_table;
      }else{
	table_stall_time[p.hash] += table[p.hash].next_time - p.timestamp;
      }
    }
    p.timestamp = table[p.hash].next_time;
    e = pair<Func, Packet>(&Global::accessTable, p);
  }
 INSERT:
  event_handler.insert(pair<double, Event>(p.timestamp, e));
}

void Global::accessCache(Packet p){
  Event e;
  if(p.timestamp >= cache[p.cache_num].next_time_read){
    p.timestamp += delay_cache;
    if(cache[p.cache_num].Access(p)){   //  HIT
      p.cache_miss = 0;
      table_hit_cache_hit ++;
      e = pair<Func, Packet>(&Global::decodePacket, p);
    }else{  //  MISS
      p.cache_miss = 1;
      table_hit_cache_miss ++;
      e = pair<Func, Packet>(&Global::accessDram, p);
    }
  }else{
    if(num_packet >= 10001){
      cache_stall_count[p.cache_num] += 1;
      if(cache[p.cache_num].next_time_read - p.timestamp >= delay_cache){
	cache_stall_time[p.cache_num] += delay_cache;
      }else{
	cache_stall_time[p.cache_num] += cache[p.cache_num].next_time_read - p.timestamp;
      }
    }	
    p.timestamp = cache[p.cache_num].next_time_read;
    e = pair<Func, Packet>(&Global::accessCache, p);
  }
  event_handler.insert(pair<double, Event>(p.timestamp, e));
}

void Global::accessDram(Packet p){
  Event e;
  dram_request_count += 1;
  if(p.timestamp >= dram->next_time_read){
    p.timestamp += delay_dram;
    dram->Read(p);
    e = pair<Func, Packet>(&Global::decodePacket, p);
  }else{
    //cout << dram->next_time - p.timestamp << endl;
    if(num_packet >= 10001){
      dram_stall_count += 1;
      if(dram->next_time_read - p.timestamp > 0.000008){
	dram_stall_time  += 0.000008;
      }else{
	dram_stall_time += dram->next_time_read - p.timestamp;
      }
    }
    p.timestamp = dram->next_time_read;
    e = pair<Func, Packet>(&Global::accessDram, p);
  }
  event_handler.insert(pair<double, Event>(p.timestamp, e));
}

void Global::decodeFirstPacket(Packet p){
  p.timestamp += delay_makehuff + delay_decode;
  Event e = pair<Func, Packet>(&Global::updateFirstPacket, p);
  event_handler.insert(pair<double, Event>(p.timestamp, e));
}

void Global::decodePacket(Packet p){
  p.timestamp += delay_decode;
  //p.current_dictsize += p.length / p.comp_ratio; //(add)辞書サイズの更新
#ifdef DONT_CACHE
  if(p.comp_len <= BYPASSSIZE){
    goto INSERT;//11/14
  }
#endif
#ifdef STREAM_SIZE_PREDICTION
  //(add)予測を超えたかの判定に用いる情報
  unsigned long new_dict_size;
  u_int current_cache_size;
  u_int current_cache_amount;
  new_dict_size = table[p.hash].GetCurrentSize(p) + (p.length /p.comp_ratio);
  current_cache_size = table[p.hash].GetCacheInfo(p).first;
  current_cache_amount = table[p.hash].GetCacheInfo(p).second;
  
  //(add)予測を超えた場合の処理
  if(new_dict_size >= current_cache_size * 1024 && current_cache_size < 32){
    map<u_int, pair<u_int, u_int> >::iterator it = num_eachcache.begin();
    while(it != num_eachcache.end()){
      if(new_dict_size < it->first * 1024){break;}
      it++;
    }
    if(it == num_eachcache.end()){it--;}
    p.info_current_cache.first = it->first;//cache_size
    p.info_current_cache.second = it->second.second; //* 32 / p.info_current_cache.first;//cache_amount(***)
    //置換防止フラグ解除
    cache[p.cache_num].Update(p);
    p.cache_miss = 1;
    p.cache_num = it->second.first - p.hash % it->second.second;//cache_size内でのcache_num
    num_prediction_miss += 1;
  } //キャッシュ番号の更新
  else{
    p.info_current_cache.first = current_cache_size;
    p.info_current_cache.second = current_cache_amount;
  }     
#endif
 INSERT:
  Event e = pair<Func, Packet>(&Global::updatePacket, p);
  event_handler.insert(pair<double, Event>(p.timestamp, e));
}

void Global::updateFirstPacket(Packet p){
  //  どのキャッシュ番号にキャッシュすればよいか？
  map<u_int, pair<u_int, u_int> >::iterator it = num_eachcache.begin();
  unsigned long ratio; // comp_len * ratio = decomp_len(予測値)
  ratio = 3.0;
#ifdef DONT_CACHE
  if(p.comp_len <= BYPASSSIZE){
    Event e;
    while(true){//DRAM access
      dram_request_count += 1;
      if(p.timestamp >= dram->next_time_write){
	p.timestamp += delay_dram;
	dram->Write(p);
	goto INSERT;
      }else{
	//cout << dram->next_time - p.timestamp << endl;
	if(num_packet >= 10001){
	  dram_stall_count += 1;
	  if(dram->next_time_write - p.timestamp > 0.000008){
	    dram_stall_time += 0.000008;
	  }else{
	    dram_stall_time += dram->next_time_write - p.timestamp;
	  }
	}
	p.timestamp = dram->next_time_write;
      }
    }
  }
#endif
  //cout << p.id << endl;
  while(it != num_eachcache.end()){
#ifdef STREAM_SIZE_PREDICTION
    if(p.comp_len * ratio < it->first * 1024){break;}  //予測有
#else
    if(p.decomp_len < it->first * 1024){break;} //予測無
#endif
    it++;
  }
  if(it == num_eachcache.end()){it--;}
  //  そのサイズのキャッシュが複数ある場合，(hash値 % そのサイズキャッシュの個数)により割り振り
  p.cache_num = it->second.first - p.hash % it->second.second; //末尾のキャッシュ番号から引く
  //cout << p.cache_num << endl;
  res[p.cache_num] += 1;
#ifdef STREAM_SIZE_PREDICTION
  p.info_current_cache.first = it->first; //(add)現在のエントリサイズを記録
  p.info_current_cache.second = it->second.second; //* 32 / p.info_current_cache.first; //(add)現在のエントリサイズを持つキャッシュの個数
#endif
  if(p.timestamp >= cache[p.cache_num].next_time_write){
    cache[p.cache_num].Insert(p);
    //12/4 ここにライトバック実装のためDRAMアクセスを入れるべき?もしくはcache.Insert内に記述か.
    while(true){//DRAM access
      dram_request_count += 1;
      if(p.timestamp >= dram->next_time_write){
	p.timestamp += delay_dram;
	dram->Write(p);
	break;
      }else{
	if(num_packet >= 10001){
	  dram_stall_count += 1;
	  if(dram->next_time_write - p.timestamp > 0.000008){
	    dram_stall_time += 0.000008;
	  }else{
	    dram_stall_time += dram->next_time_write - p.timestamp;
	  }
	}
	p.timestamp = dram->next_time_write;
      }
    }
    while(true){
      if(p.timestamp >= table[p.hash].next_time){
	table[p.hash].Update(p);
	break;
      }else{
	if(num_packet >= 10001){
	  table_stall_time_total += table[p.hash].next_time - p.timestamp;
	}
	p.timestamp = table[p.hash].next_time;
      }
    }
  INSERT:
    mod[p.hash].Update(p);
    mod[p.hash].status = 0;
    end_time = p.timestamp;
    if(mod[p.hash].q.size() != 0){
      //cout << "a" << endl;
      Event e = pair<Func, Packet>(&Global::outputQueue, p);
      event_handler.insert(pair<double, Event>(p.timestamp, e));
    }else{
      //cout << "debug.upfp: " << p.cache_num << endl;
      inputPacket(p);
    }
  }else{
    //cout << "cache stall w" << endl;
    cache_stall_time_total += cache[p.cache_num].next_time_write - p.timestamp;
    p.timestamp = cache[p.cache_num].next_time_write;
    Event e = pair<Func, Packet>(&Global::updateFirstPacket, p);
    event_handler.insert(pair<double, Event>(p.timestamp, e));
  }
}


void Global::updatePacket(Packet p){
#ifdef DONT_CACHE
  if(p.comp_len <= BYPASSSIZE){
    //DRAM access
    while(true){
      dram_request_count += 1;
      if(p.timestamp >= dram->next_time_write){
	p.timestamp += delay_dram;
	dram->Write(p);
	goto INSERT;
      }else{
	if(num_packet >= 10001){
	  dram_stall_count += 1;
	  if(dram->next_time_write - p.timestamp > 0.000008){
	    dram_stall_time += 0.000008;
	  }else{
	    dram_stall_time += dram->next_time_write - p.timestamp;
	  }
	}
	p.timestamp = dram->next_time_write;
      }
    }
  }
#endif
  if(p.timestamp >= cache[p.cache_num].next_time_write){
    while(true){//DRAM access
      dram_request_count += 1;
      if(p.timestamp >= dram->next_time_write){
	p.timestamp += delay_dram;
	dram->Write(p);
	break;
      }else{
	if(num_packet >= 10001){
	  dram_stall_count += 1;
	  if(dram->next_time_write - p.timestamp > 0.000008){
	    dram_stall_time += 0.000008;
	  }else{
	    dram_stall_time += dram->next_time_write - p.timestamp;
	  }
	}
	p.timestamp = dram->next_time_write;
      }
    }
    p.timestamp += delay_cache;
    if(p.cache_miss == 1){
      cache[p.cache_num].Insert(p);
      //cout << p.id << endl;
    }else{
      cache[p.cache_num].Update(p);
    }
    //cout << "ID:" << p.id << "\tmiss:" << p.cache_miss;
    //cout << "\tcache[num,line]:" << p.cache_num << "," << p.line_num << endl;
  INSERT:
    //Tableアクセス処理
    while(true){//Acsess
      if(p.timestamp >= table[p.hash].next_time){
	p.timestamp += delay_table;
	table[p.hash].Update(p);
	mod[p.hash].Update(p);
	mod[p.hash].status = 0;
	end_time = p.timestamp;
	break;
      }else{//Wait
	if(num_packet >= 10001){
	  table_stall_time_total += table[p.hash].next_time - p.timestamp;
	}
	p.timestamp = table[p.hash].next_time;
      }
    }
    
    if(mod[p.hash].q.size() != 0){
      //cout << "A" << endl;
      Event e = pair<Func, Packet>(&Global::outputQueue, p);
      event_handler.insert(pair<double, Event>(p.timestamp, e));
    }else{
      inputPacket(p);
    }
  }else{
    //cout << "cache atall w" << endl; 
    cache_stall_time_total += cache[p.cache_num].next_time_write - p.timestamp;
    p.timestamp = cache[p.cache_num].next_time_write;
    Event e = pair<Func, Packet>(&Global::updatePacket, p);
    event_handler.insert(pair<double, Event>(p.timestamp, e));
  }
}

void Global::processEvent(){
    multimap<double, Event>::iterator it = event_handler.begin();
    /*	if(it->first > (double)res_sec){
		showResult();
		res_sec++;
		}*/
    (this->*(it->second.first))(it->second.second);
    event_handler.erase(it);
}

void Global::through(Packet p){
  mod[p.hash].status = 0;
  num_of_through += 1;
  inputPacket(p);
}

double Global::getReadedPacket(){
  return num_of_ReadedPacket;
}
 

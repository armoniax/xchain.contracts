/*
  App configuration example created by eoswebnetbp1
*/
const path = require('path');
let config = {};

// production mod
config.PROD = true;

config.toInt = 100000000; //precison: 8
config.coin = 'AMAX';
// console.log("process.env.MONGO_URI: ===========>>>>>>>>>>", process.env)

// cron processes (aggregation of main stat - actions, transactions, accounts, analytics)
config.CRON = true;
config.CRON_API = process.env.CRON_API;

// enable for private network (daemon for Actions, Accounts)
config.CUSTOM_GLOBA_STATS = false;

// reserved nodes
config.endpoints = [
  'http://172.20.142.169:18888'
];

// eosjs
config.eosConfig = {
  chainId: "208dacab3cd2e181c86841613cf05d9c60786c677e4ce86b266d0a58884968f7",
  keyProvider: "",
  httpEndpoint: config.endpoints[0],
  expireInSeconds: 60,
  broadcast: true,
  debug: false,
  sign: true,
  logger: {
    error: console.error
  }
};

// api url for producers list
config.customChain = process.env.CUSTOMCHAIN || 'http://172.20.142.169:18888';

config.apiV = 'v1'; // api version
config.RAM_UPDATE = 5 * 60 * 1000; // time for ram update - /api/api.*.socket
config.HISTORY_UPDATE = 5 * 60 * 1000; // time for stats update - /api/api.*.socket 
config.MAX_BUFFER = 500000; // max buffer size for child processes (kb) - /crons
config.blockUpdateTime = 900; // mainpage upades frequency  - /api/api.*.socket in ml sec
config.offsetElementsOnMainpage = 10; // blocks on mainpage
config.limitAsync = 30; // max threads for async.js module  
config.updateTPS = 1000;


module.exports = config;

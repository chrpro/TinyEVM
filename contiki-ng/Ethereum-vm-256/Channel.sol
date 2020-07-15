pragma solidity ^0.5.11;
contract HashLock {
    uint money_lock = 100;
    bytes32 public hashClose = 0x000;
    bytes32 public hashLock = 0x000;
    function () public payable {

    }

    function step_money(string _primage_h,uint charge, string _new_h) public {
        require(sha256(_primage_h) == hashLock,'wrong unlock hash');
        money_lock = 100 - charge;
        hashLock = _new_h;
        selfdestruct(msg.sender);
    }

    function close(string _primage_h) public {
        require(sha256(_primage_h) == hashClose,'wrong unlock hash');
        selfdestruct(msg.sender);
    }

}
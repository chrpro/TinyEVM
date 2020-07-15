
pragma solidity ^0.5.0;
contract Agreement {
    uint64 public PaymentChannel;
    uint money_lock = 100;
    bytes32 public hashClose = 0x000;
    string public hashLock = '0x000';
    
    constructor(uint64 LockMoney) public {
        PaymentChannel = LockMoney;
    }
    function step_money(uint charge, string memory _new_h) public {
        // require(sha256(_primage_h) == hashLock,'wrong unlock hash');
        money_lock = 100 - charge;
        hashLock = _new_h;
        selfdestruct(msg.sender);
    }
}

contract Factory {
    Agreement newContract;

    function createContract (uint64 Money) public  {
        newContract = new Agreement(Money);
    } 
}


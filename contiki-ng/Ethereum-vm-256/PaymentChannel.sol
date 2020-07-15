pragma solidity ^0.5.11;


contract PaymentChannel {
    struct TemplateContract {
        address sender;
        address receiver;
        address tokenContract;
        uint256 amount;
        bytes32 logical_clock;
        uint256 timelock;
        bool withdrawn;
        bool refunded;
        bytes32 preimage;
    }
  address payable public sender;
  address payable public recipient;
  uint public expiration;
  uint public sensor_data;

  constructor(address payable _recipient, uint duration) public payable {
    sender = msg.sender;
    recipient = _recipient;
    expiration = now + duration;
    // assembly{
    //     let a:= 0xbeef
    //     // pop(a)
    //     invalid()
    //     sstore(0x04,a)
    // }
  }

  function isValidSignature(uint amount, bytes memory signature)
    internal
    view
    returns (bool)
  {
    bytes32 message = prefixed(keccak256(abi.encodePacked(this, amount)));

    return recoverSigner(message, signature) == sender;
  }

  // The recipient can close the channel at any time by presenting a
  // signed amount from the sender. The recipient will be sent that amount,
  // and the remainder will go back to the sender
  function close(uint amount, bytes memory signature) public payable {
    require(msg.sender == recipient);
    require(isValidSignature(amount, signature));

    recipient.transfer(amount);
    selfdestruct(sender);
  }

  // Sender can extend the expiration at any time
  function extend(uint newExpiration) public {
    require(msg.sender == sender);
    require(newExpiration > expiration);

    expiration = newExpiration;
  }

  // If the timeout is reached without the recipient closing the channel,
  // then the Ether is released back to the sender.
  function claimTimeout() public {
    require(now >= expiration);
    selfdestruct(sender);
  }

  function recoverSigner(bytes32 message, bytes memory sig)
    internal
    pure
    returns (address)
  {
    (uint8 v, bytes32 r, bytes32 s) = splitSignature(sig);

    // If you are using Remix IDE, you may need to hard code
    v = 0x1b;// due to a bug
    return ecrecover(message, v, r, s);
  }

  /// builds a prefixed hash to mimic the behavior of eth_sign.
  function prefixed(bytes32 hash) internal pure returns (bytes32) {
    return keccak256(abi.encodePacked("\x19Ethereum Signed Message:\n32", hash));
  }

  function splitSignature(bytes memory sig)
    internal
    pure
    returns (uint8 v, bytes32 r, bytes32 s)
  {
    require(sig.length == 65);

    assembly {
      // first 32 bytes, after the length prefix
      r := mload(add(sig, 32))
      // second 32 bytes
      s := mload(add(sig, 64))
      // final byte (first byte of the next 32 bytes)
      v := byte(0, mload(add(sig, 96)))
    }

    return (v, r, s);
  }
}
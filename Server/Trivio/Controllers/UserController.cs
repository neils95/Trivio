﻿using AutoMapper;
using AutoMapper.QueryableExtensions;
using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Data.Entity.Infrastructure;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Web.Http;
using System.Web.Http.Description;
using Trivio.Models;
using Trivio.Models.DTOs;

namespace Trivio.Controllers
{
	[RoutePrefix("User")]
    public class UserController : ApiController
    {
        private TrivioContext db = new TrivioContext();

		//POST: User/Register
		[HttpPost,Route("Register", Name = "RegisterUser")]
		[ResponseType(typeof(UserPublicDTO))]
		public async Task<IHttpActionResult> RegisterUser(UserRegistrationDTO newUser)
		{
			if (!ModelState.IsValid)
			{
				return BadRequest(ModelState);
			}

			//Add user to Db.
			try 
			{
				Regex rx = new Regex(@"^[-!#$%&'*+/0-9=?A-Z^_a-z{|}~](\.?[-!#$%&'*+/0-9=?A-Z^_a-z{|}~])*@[a-zA-Z](-?[a-zA-Z0-9])*(\.[a-zA-Z](-?[a-zA-Z0-9])*)+$");
				if(rx.IsMatch(newUser.Email)) //valid email
				 {
					var user = Mapper.Map<User>(newUser);
					db.Users.Add(user);
					await db.SaveChangesAsync();

					//Check Header for path to new resource
					return CreatedAtRoute(
						"RegisterUser",
						new { id = user.Id },
						Mapper.Map<UserPublicDTO>(user)
					);

				}
				else 
				{
					throw new Exception("Invalid Email Id format");
				}

			}
			catch {
				return BadRequest(String.Format("Account with email {0} already exists or invalid email",newUser.Email));
			}
		}

		//POST: User/Login
		[HttpPost,Route("Login",Name ="LoginUser")]
		[ResponseType(typeof(UserPublicDTO))]
		public async Task<IHttpActionResult> LoginUser(UserLoginDTO user)
		{
			User existingUser = await db.Users.FirstOrDefaultAsync(x=>(x.Email==user.Email && x.Password==user.Password));
			if (existingUser == null)
			{
				return NotFound();
			}

			return Ok(Mapper.Map<UserPublicDTO>(existingUser));
		}

		//GET: User/History/{UserId}
		[HttpGet]
		[Route("History/{id}",Name ="UserTriviaHistory")]
		public async Task<IHttpActionResult> GetUserTriviaHistory(int id)
		{
			//Check if User exists
			User user = await db.Users.FindAsync(id);
			if (user == null)
			{
				return BadRequest("Invalid UserId");
			}

			
			var userTriviaHistory = db.UserTriviaHistories
									.Include(b => b.Trivia)
									.Where(x => x.UserId == id)
									.Select(m=>new
										{
											Trivia = m.Trivia,
											UserVote=m.UserVote
										}
									).ToList();
			
			return Ok(new { 
				TriviaHistory = userTriviaHistory
			});
		}

		//PUT: User/History/{UserId}/{Count}
		[HttpPut, Route("History/{userId}/{triviaCount}")]
		[ResponseType(typeof(void))]
		public async Task<IHttpActionResult> UpdateUserTriviaHistory(int userId, int triviaCount)
		{
			User user = await db.Users.FindAsync(userId);

			//Check if triviacount is greater than the number of facts user has heard.
			if (user == null || triviaCount < 1 || (user.TriviaHistoryCount + triviaCount > user.TriviaCount))
			{
				return BadRequest("Invalid User Id/Trivia Count must be a number");
			}


			for (int i = user.TriviaHistoryCount + 1; i <= user.TriviaHistoryCount + triviaCount; i++)
			{
				db.UserTriviaHistories.Add(new UserTriviaHistory
					{
						TriviaId = i,
						UserId = userId,
						UserVote = 0
					}
				);
			}
			user.TriviaHistoryCount+=triviaCount;


			await db.SaveChangesAsync();
			return Ok();
		}

		//----------------------------------------------------------------------------------

		// GET: Users
		[HttpGet]
		public IQueryable<UserPublicDTO> GetUser()
        {
            return db.Users.ProjectTo<UserPublicDTO>();
        }

        // GET: Users/5
        [ResponseType(typeof(User))]
        public async Task<IHttpActionResult> GetUser(int id)
        {
            User user = await db.Users.FindAsync(id);
            if (user == null)
            {
                return BadRequest("Invalid User Id");
            }

            return Ok(Mapper.Map<UserPublicDTO>(user));
        }

        // PUT: User/5
        [ResponseType(typeof(void))]
        public async Task<IHttpActionResult> PutUser(int id, User user)
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            if (id != user.Id)
            {
                return BadRequest();
            }

            db.Entry(user).State = EntityState.Modified;

            try
            {
                await db.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!UserExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return StatusCode(HttpStatusCode.NoContent);
        }

        // DELETE: User/5
        [ResponseType(typeof(User))]
        public async Task<IHttpActionResult> DeleteUser(int id)
        {
            User user = await db.Users.FindAsync(id);
            if (user == null)
            {
                return NotFound();
            }

            db.Users.Remove(user);
            await db.SaveChangesAsync();

            return Ok(user);
        }
		
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                db.Dispose();
            }
            base.Dispose(disposing);
        }

        private bool UserExists(int id)
        {
            return db.Users.Count(e => e.Id == id) > 0;
        }
    }
}